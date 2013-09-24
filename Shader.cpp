#include "Shader.h"

Shader::Shader(const std::string& vs, const std::string& fs)
{
#if 0
	prog = glCreateProgramObjectARB();
	vert = 0;
	frag = 0;

	std::ifstream vs_ifs(vs);
	std::string vs_content( (std::istreambuf_iterator<char>(vs_ifs) ),
						   (std::istreambuf_iterator<char>()    ) );

	std::ifstream fs_ifs(vs);
	std::string fs_content( (std::istreambuf_iterator<char>(vs_ifs) ),
						   (std::istreambuf_iterator<char>()    ) );

	// Compile the vertex shader.

	if (vert_str.length() > 0)
	{
		const GLcharARB *p = (const GLcharARB *) vert_str.c_str();

		vert = glCreateShaderObjectARB(GL_VERTEX_SHADER_ARB);

		glShaderSourceARB (vert, 1, &p, 0);
		glCompileShaderARB(vert);

		check_log(vert);
	}

	// Compile the frag shader.

	if (frag_str.length() > 0)
	{
		const GLcharARB *p = (const GLcharARB *) frag_str.c_str();

		frag = glCreateShaderObjectARB(GL_FRAGMENT_SHADER_ARB);

		glShaderSourceARB (frag, 1, &p, 0);
		glCompileShaderARB(frag);

		check_log(frag);
	}

	// Link these shader objects to a program object.

	glAttachObjectARB(prog, vert);
	glAttachObjectARB(prog, frag);

	glLinkProgramARB(prog);   check_log(prog);

	GL_CHECK();
#endif
}
