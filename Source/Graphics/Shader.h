#pragma once

typedef unsigned int GLuint;

class CShader
{
public:
	CShader(const std::string& vertexShader, const std::string& fragmentShader);
	CShader(GLuint vs, GLuint fs);

public:
	~CShader();

	static CShader* Load(const std::string& VSFile, const std::string& FSFile);
	static void ReloadAll();
	static void UnloadAll();

	void Use() const;

	void SetBool(const char* name, bool value) const;
    void SetInt(const char* name, int value) const;
	void SetFloat(const char* name, float value) const;
	void SetMatrix(const char* name, const float* value) const;

	bool const operator==(const CShader& otherShader) const
	{
		return mID == otherShader.mID;
	}

public:
	const GLuint GetID() const { return mID; };

private:
	static bool CompileShader(GLuint type, const std::string& shader, GLuint* outID);
	static const std::string ReadShader(const std::string& file);

private:
	static std::map<const std::string, GLuint> msLoadedShaders;
	static std::vector<CShader> msShaders;

	GLuint mID;
	std::string mVSFile;
	std::string mFSFile;
	
};

