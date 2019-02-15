#include "Shader.h"

#include "Debug/Debug.h"

#include <algorithm>
#include <iostream>
#include <fstream>
#include <sstream>

std::map<const std::string, GLuint> CShader::msLoadedShaders;
std::vector<CShader> CShader::msShaders;

CShader::CShader(const std::string& vertexShader, const std::string& fragmentShader)
{
	bool success = true;

	GLuint vs;
	success &= CShader::CompileShader(GL_VERTEX_SHADER, vertexShader, &vs);

	GLuint fs;
	success &= CShader::CompileShader(GL_FRAGMENT_SHADER, fragmentShader, &fs);
	
	if (success)
	{
		CShader(vs, fs);
	}

	glDeleteShader(vs);
	glDeleteShader(fs);
}

CShader::CShader(GLuint vs, GLuint fs)
{
	GLuint id = glCreateProgram();

	glAttachShader(id, vs);
	glAttachShader(id, fs);
	glLinkProgram(id);
	glValidateProgram(id);

	glDetachShader(id, vs);
	glDetachShader(id, fs);

	mID = id;
}

CShader::~CShader()
{
	//CShader::msShaders.erase(std::remove(CShader::msShaders.begin(), CShader::msShaders.end(), *this), CShader::msShaders.end());
	glDeleteProgram(mID);
}

CShader* CShader::Load(const std::string& VSFile, const std::string& FSFile)
{
	if (CShader::msLoadedShaders.find(VSFile) == CShader::msLoadedShaders.end())
	{
		auto VSCode = CShader::ReadShader(VSFile);

		GLuint shaderId;
		if (CShader::CompileShader(GL_VERTEX_SHADER, VSCode, &shaderId))
		{
			CShader::msLoadedShaders[VSFile] = shaderId;
		}
	}

	if (CShader::msLoadedShaders.find(FSFile) == CShader::msLoadedShaders.end())
	{
		auto FSCode = CShader::ReadShader(FSFile);

		GLuint shaderId;
		if (CShader::CompileShader(GL_FRAGMENT_SHADER, FSCode, &shaderId))
		{
			CShader::msLoadedShaders[FSFile] = shaderId;
		}
	}

	for (CShader& shader : CShader::msShaders)
	{
		if (shader.mFSFile == FSFile && shader.mVSFile == VSFile)
		{
			return &shader;
		}
	}

	CShader& s = CShader::msShaders.emplace_back(CShader::msLoadedShaders[VSFile], CShader::msLoadedShaders[FSFile]);
	s.mVSFile = VSFile;
	s.mFSFile = FSFile;

	return &s;
}

void CShader::ReloadAll()
{
	for (CShader& shader2 : CShader::msShaders)
	{
		CShader* shader = &shader2;

		GLuint iVS;
		if (CShader::CompileShader(GL_VERTEX_SHADER, CShader::ReadShader(shader->mVSFile), &iVS))
		{
			if (CShader::msLoadedShaders.find(shader->mVSFile) != CShader::msLoadedShaders.end())
			{
				glDeleteShader(CShader::msLoadedShaders[shader->mVSFile]);
				CShader::msLoadedShaders[shader->mVSFile] = iVS;
			}
		}

		GLuint iFS;
		if (CShader::CompileShader(GL_FRAGMENT_SHADER, CShader::ReadShader(shader->mFSFile), &iFS))
		{
			if (CShader::msLoadedShaders.find(shader->mFSFile) != CShader::msLoadedShaders.end())
			{
				glDeleteShader(CShader::msLoadedShaders[shader->mFSFile]);
				CShader::msLoadedShaders[shader->mFSFile] = iFS;
			}
		}

		glDeleteProgram(shader->mID);
		GLuint shaderId = glCreateProgram();

		glAttachShader(shaderId, iVS);
		glAttachShader(shaderId, iFS);
		glLinkProgram(shaderId);
		glValidateProgram(shaderId);

		glDetachShader(shaderId, iVS);
		glDetachShader(shaderId, iFS);

		shader->mID = shaderId;
	}
}

void CShader::UnloadAll()
{
	for (auto ls : CShader::msLoadedShaders)
	{
		glDeleteShader(ls.second);
	}

	CShader::msLoadedShaders.clear();
}

void CShader::Use() const
{
	glUseProgram(mID);
}

void CShader::SetBool(const std::string& name, bool value) const
{
	GLuint ul = glGetUniformLocation(mID, name.c_str());
	if (ul != -1)
	{
		glUniform1i(ul, value);
	}
}

void CShader::SetInt(const std::string& name, int value) const
{
	GLuint ul = glGetUniformLocation(mID, name.c_str());
	if (ul != -1)
	{
		glUniform1i(ul, value);
	}
}

void CShader::SetFloat(const std::string& name, float value) const
{
	GLuint ul = glGetUniformLocation(mID, name.c_str());
	if (ul != -1)
	{
		glUniform1f(ul, value);
	}
}

void CShader::SetMatrix(const std::string& name, const float* value) const
{
	GLuint ul = glGetUniformLocation(mID, name.c_str());
	if (ul != -1)
	{
		glUniformMatrix4fv(ul, 1, GL_FALSE, value);
	}
}

bool CShader::CompileShader(GLuint type, const std::string& shader, GLuint* outID)
{
	GLuint id = glCreateShader(type);
	const char* sSource = shader.c_str();

	glShaderSource(id, 1, &sSource, NULL);
	glCompileShader(id);

	int success;
	glGetShaderiv(id, GL_COMPILE_STATUS, &success);
	if (success == GL_FALSE)
	{
		int iLength;
		glGetShaderiv(id, GL_INFO_LOG_LENGTH, &iLength);

		char* sMessage = (char*)alloca(iLength * sizeof(char));
		glGetShaderInfoLog(id, 512, NULL, sMessage);

		glDeleteShader(id);

		std::cout << "Shader Compile Error: " << std::endl << sMessage << std::endl;

		return false;
	}

	*outID = id;
	return true;
}

const std::string CShader::ReadShader(const std::string& file)
{
	std::string code;

	std::ifstream fileStream;
	fileStream.exceptions(std::ifstream::failbit | std::ifstream::badbit);

	try
	{
		fileStream.open(file);

		std::stringstream stringStream;
		stringStream << fileStream.rdbuf();

		fileStream.close();

		return stringStream.str();
	}
	catch (std::ifstream::failure e)
	{
		std::cout << "Shader Read Error: " << file << std::endl;
	}

	return "";
}
