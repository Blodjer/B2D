#include "B2D_pch.h"
#include "OpenGLGHI.h"

#include <GL/glew.h>
#include <GLFW/glfw3.h>

OpenGLGHI::OpenGLGHI()
{

}

OpenGLGHI::~OpenGLGHI()
{

}

void OpenGLGHI::Clear()
{
    glClearColor(0.7f, 0, 0.7f, 1);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void OpenGLGHI::Swap()
{
    //glfwSwapBuffers(mContext);
}