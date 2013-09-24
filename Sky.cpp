#include "Sky.h"
#include <iostream>
#include <IL/il.h>


class SolidSphere
{
protected:
	std::vector<GLfloat> vertices;
	std::vector<GLfloat> normals;
	std::vector<GLfloat> texcoords;
	std::vector<GLushort> indices;

public:
	SolidSphere(float radius, unsigned int rings, unsigned int sectors)
	{
		float const R = 1./(float)(rings-1);
		float const S = 1./(float)(sectors-1);
		int r, s;

		vertices.resize(rings * sectors * 3);
		normals.resize(rings * sectors * 3);
		texcoords.resize(rings * sectors * 2);
		std::vector<GLfloat>::iterator v = vertices.begin();
		std::vector<GLfloat>::iterator n = normals.begin();
		std::vector<GLfloat>::iterator t = texcoords.begin();
		for(r = 0; r < rings; r++) for(s = 0; s < sectors; s++) {
				float const y = sin( -M_PI_2 + M_PI * r * R );
				float const x = cos(2*M_PI * s * S) * sin( M_PI * r * R );
				float const z = sin(2*M_PI * s * S) * sin( M_PI * r * R );

				*t++ = s*S;
				*t++ = r*R;

				*v++ = x * radius;
				*v++ = y * radius;
				*v++ = z * radius;

				*n++ = x;
				*n++ = y;
				*n++ = z;
		}

		indices.resize(rings * sectors * 4);
		std::vector<GLushort>::iterator i = indices.begin();
		for(r = 0; r < rings-1; r++) for(s = 0; s < sectors-1; s++) {
				*i++ = r * sectors + s;
				*i++ = r * sectors + (s+1);
				*i++ = (r+1) * sectors + (s+1);
				*i++ = (r+1) * sectors + s;
		}
	}

	void draw(GLfloat x, GLfloat y, GLfloat z)
	{
		glMatrixMode(GL_MODELVIEW);
		glPushMatrix();
		glTranslatef(x,y,z);

		glEnableClientState(GL_VERTEX_ARRAY);
		glEnableClientState(GL_NORMAL_ARRAY);
		glEnableClientState(GL_TEXTURE_COORD_ARRAY);

		glVertexPointer(3, GL_FLOAT, 0, &vertices[0]);
		glNormalPointer(GL_FLOAT, 0, &normals[0]);
		glTexCoordPointer(2, GL_FLOAT, 0, &texcoords[0]);
		glDrawElements(GL_QUADS, indices.size(), GL_UNSIGNED_SHORT, &indices[0]);
		glPopMatrix();

		glDisableClientState(GL_VERTEX_ARRAY);
		glDisableClientState(GL_NORMAL_ARRAY);
		glDisableClientState(GL_TEXTURE_COORD_ARRAY);
	}
};

GLuint loadTexture(const std::string& name)
{

	ilInit();

	GLuint texId;
	glGenTextures(1, &texId);

	ILuint ilId;
	ilGenImages(1, &ilId);

	// DevIL-Bild Name binden
	ilBindImage(ilId);
	ilEnable(IL_ORIGIN_SET);
	ilOriginFunc(IL_ORIGIN_LOWER_LEFT);

	if (!ilLoadImage(name.c_str()))
	{
		return 0;
	}
	// Bild in RGBA umwandeln
	ilConvertImage(IL_RGBA, IL_UNSIGNED_BYTE);
	glActiveTexture(GL_TEXTURE0);
	// OpenGL Texture erstellen und laden
	glBindTexture(GL_TEXTURE_2D, texId);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

	//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, ilGetInteger(IL_IMAGE_WIDTH), ilGetInteger(IL_IMAGE_HEIGHT), 0, GL_RGBA, GL_UNSIGNED_BYTE, ilGetData());

	ilDeleteImage(ilId);

	return texId;
}

//-----------------------------------------------------------------------------
Sky::Sky() : Entity("bla")
{
	shader_.reset(new Shader("Shader/sky.vert","Shader/sky.frag"));

	tex_glow=loadTexture("Shader/glow.png");
	tex_sky=loadTexture("Shader/sky.png");

	tex_stars=loadTexture("Shader/stars2.png");

	timeOfDay_=19;
	secPerH_=2;
}
//-----------------------------------------------------------------------------
void Sky::setTimeOfDay(double t)
{
	timeOfDay_=t;
}
//-----------------------------------------------------------------------------
void Sky::setSecondsPerHour(double secPerH)
{
	secPerH_=secPerH;
}
//-----------------------------------------------------------------------------
void Sky::update(double dt)
{
	timeOfDay_+=dt/secPerH_;

	//sunPos << sin(glfwGetTime()/6), 0,cos(glfwGetTime()/6);
	sunPos_ << sin((timeOfDay_+12)/24.0*2*M_PI),0,cos((timeOfDay_+12)/24.0*2*M_PI);
	sunPos_*=150;
}
//-----------------------------------------------------------------------------
void Sky::render()
{
	glUseProgram(shader_->handle());

	int loc;
	if ((loc = glGetUniformLocationARB(shader_->handle(), "glow")) >= 0)
		glUniform1iARB(loc, 0);
	if ((loc = glGetUniformLocationARB(shader_->handle(), "color")) >= 0)
		glUniform1iARB(loc, 1);
	if ((loc = glGetUniformLocationARB(shader_->handle(), "stars")) >= 0)
		glUniform1iARB(loc, 2);

	glActiveTexture( GL_TEXTURE0 );
	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, tex_glow);
	glActiveTexture( GL_TEXTURE1 );
	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, tex_sky);
	glActiveTexture( GL_TEXTURE2 );
	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, tex_stars);

	if ((loc = glGetUniformLocationARB(shader_->handle(),"lightPos")) >= 0)
		glUniform3fARB(loc, sunPos_.x(),sunPos_.y(),sunPos_.z());

	glMatrixMode(GL_MODELVIEW);
	glDisable(GL_DEPTH_TEST);
	glDepthRange(1,1);
	glPushMatrix();
	glScalef(100,100,100);
	static SolidSphere skydome(1,20,20);
	skydome.draw(0,0,0);
	glPopMatrix();
	glDepthRange(0,1);
	glEnable(GL_DEPTH_TEST);

	glUseProgram(0);

	//SolidSphere s(10,5,5);
	//s.draw(sunPos.x(),sunPos.y(),sunPos.z());

	glActiveTexture( GL_TEXTURE2 );
	glDisable(GL_TEXTURE_2D);
	glActiveTexture( GL_TEXTURE1 );
	glDisable(GL_TEXTURE_2D);
	glActiveTexture( GL_TEXTURE0 );
	glDisable(GL_TEXTURE_2D);
}
//-----------------------------------------------------------------------------
