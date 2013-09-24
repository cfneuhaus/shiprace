#ifndef SHADER_H
#define SHADER_H

#include <string>
#include <GL/glew.h>
#include <GL/glfw.h>

class Shader
{
public:
	Shader(const std::string& vs, const std::string& fs);

	void checkLog(const std::string& src, GLhandleARB handle);

	GLhandleARB handle() { return prog; }

private:
	GLhandleARB vert;
	GLhandleARB frag;
	GLhandleARB prog;
};

#endif
