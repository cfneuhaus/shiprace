#include "Shader.h"
#include <fstream>

#include <iostream>

//-----------------------------------------------------------------------------
Shader::Shader(const std::string& vs, const std::string& fs)
{
	prog = glCreateProgramObjectARB();
	vert = 0;
	frag = 0;

	std::ifstream vs_ifs(vs);
	std::string vs_content((std::istreambuf_iterator<char>(vs_ifs)), (std::istreambuf_iterator<char>()));

	std::ifstream fs_ifs(fs);
	std::string fs_content((std::istreambuf_iterator<char>(fs_ifs)), (std::istreambuf_iterator<char>()));

	// Compile the vertex shader.

	if (vs_content.length() > 0)
	{
		const GLcharARB *p = (const GLcharARB *) vs_content.c_str();

		vert = glCreateShaderObjectARB(GL_VERTEX_SHADER_ARB);

		glShaderSourceARB (vert, 1, &p, 0);
		glCompileShaderARB(vert);

		checkLog(vert);
	}

	// Compile the frag shader.

	if (fs_content.length() > 0)
	{
		const GLcharARB *p = (const GLcharARB *) fs_content.c_str();

		frag = glCreateShaderObjectARB(GL_FRAGMENT_SHADER_ARB);

		glShaderSourceARB (frag, 1, &p, 0);
		glCompileShaderARB(frag);

		checkLog(frag);
	}

	// Link these shader objects to a program object.

	glAttachObjectARB(prog, vert);
	glAttachObjectARB(prog, frag);

	glLinkProgramARB(prog);   checkLog(prog);

	//GL_CHECK();
}
//-----------------------------------------------------------------------------
void Shader::checkLog(GLhandleARB handle)
{
	char *log;
	GLint len;

	// Dump the contents of the log, if any.
	glGetObjectParameterivARB(handle, GL_OBJECT_INFO_LOG_LENGTH_ARB, &len);

	if ((len > 1) && (log = new char[len + 1]))
	{
		glGetInfoLogARB(handle, len, NULL, log);

		std::cerr << log << std::endl;

		delete [] log;
	}
}
//-----------------------------------------------------------------------------
