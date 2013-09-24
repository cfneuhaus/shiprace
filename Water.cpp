#include "Water.h"
#include "Sky.h"
#include <Eigen/Geometry>

#include <GL/glew.h>
#include <GL/glfw.h>

#include "TextureLoader.h"

//-----------------------------------------------------------------------------
Water::Water(int surfaceSize, double sizePerCell)
	:
	  surfaceSize_(surfaceSize), sizePerCell_(sizePerCell),
	  reflectedSky_(nullptr)
{
	numVertices_=surfaceSize_*surfaceSize_;
	numFaces_=(surfaceSize_-1)*(surfaceSize_-1)*2;

	initGeometry();

	shader_.reset(new Shader("Shader/water.vert","Shader/water.frag"));

	tex_glow=loadTexture("Shader/glow.png");
	tex_sky=loadTexture("Shader/sky.png");

	tex_stars=loadTexture("Shader/stars2.png");

	tex_detailWaves=loadTexture("Shader/waves_n.png",GL_REPEAT);

}
//-----------------------------------------------------------------------------
double Water::computeHeightAtXY(const Eigen::Vector2d& pos) const
{
	//Eigen::Vector2d start=pos;

	Eigen::Vector2f posf=pos.cast<float>();

	double minDist=(vertices_[0].segment<2>(0)-posf).norm();
	size_t minIndex=0;
	for (size_t i=1;i<vertices_.size();i++)
	{
		const double dist=(vertices_[i].segment<2>(0)-posf).norm();
		if (dist<minDist)
		{
			minDist=dist;
			minIndex=i;
		}
	}
	return vertices_[minIndex].z();
}
//-----------------------------------------------------------------------------
void Water::initGeometry()
{
	vertices_.resize(numVertices_);
	normals_.resize(numVertices_);

	// Generiere Vertices einer planaren Wasseroberfläche in der x-y-Ebene
	{
		int i=0;
		for(int y=(-1)*(surfaceSize_ / 2) + 1; y <= (surfaceSize_ / 2); y++)
			for (int x=(-1)*(surfaceSize_ / 2) + 1; x <= (surfaceSize_ / 2); x++)
				vertices_[i++] << x * sizePerCell_, y * sizePerCell_, 0;

		glGenBuffers(1, &vertexVBO_);
		glBindBuffer(GL_ARRAY_BUFFER, vertexVBO_);
		// allocate space for vertices+normals
		glBufferData(GL_ARRAY_BUFFER, sizeof(float) * numVertices_ * 3 * 2, 0, GL_STATIC_DRAW);
		// update just vertices
		glBufferSubData(GL_ARRAY_BUFFER, 0,sizeof(float) * numVertices_ * 3, &vertices_[0].x());//, GL_STATIC_DRAW);
	}


	indices_.resize(numFaces_*3);

	int i = 0;

	for(int y=0; y < surfaceSize_ - 1; y++)
	{
		for (int x=0; x < surfaceSize_ - 1; x++)
		{
			indices_[i++]=y*surfaceSize_ + x;
			indices_[i++]=(y+1)*surfaceSize_ + x;
			indices_[i++]=y*surfaceSize_ + x + 1;

			indices_[i++]=(y+1) * surfaceSize_ + x;
			indices_[i++]=(y+1) * surfaceSize_ + x + 1;
			indices_[i++]=y*surfaceSize_ + x + 1;
		}
	}

	glGenBuffers(1, &indexVBO_);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexVBO_);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices_.size() * sizeof(unsigned int), &indices_[0], GL_STATIC_DRAW);

	computeNormals();
}
//-----------------------------------------------------------------------------
void Water::computeNormals()
{
	for (int i=0;i<numVertices_;i++)
		normals_[i]=Eigen::Vector3f::Zero();
	for (size_t i=0;i<indices_.size();i+=3)
	{
		Eigen::Vector3f a=vertices_[indices_[i+0]];
		Eigen::Vector3f b=vertices_[indices_[i+1]];
		Eigen::Vector3f c=vertices_[indices_[i+2]];

		Eigen::Vector3f n=-(b-a).cross(c-a).normalized();
		normals_[indices_[i+0]]+=n;
		normals_[indices_[i+1]]+=n;
		normals_[indices_[i+2]]+=n;
	}
	for (int i=0;i<numVertices_;i++)
		normals_[i]=normals_[i].normalized();
}
//-----------------------------------------------------------------------------
void Water::update(double dt)
{
	static double kk=0;
	int i=0;
	for(int y=(-1)*(surfaceSize_ / 2) + 1; y <= (surfaceSize_ / 2); y++)
		for (int x=(-1)*(surfaceSize_ / 2) + 1; x <= (surfaceSize_ / 2); x++)
			vertices_[i++] << x * sizePerCell_, y * sizePerCell_, kk;
	kk+=0.01;

	updateGeometry();
}
//-----------------------------------------------------------------------------
void Water::updateGeometry()
{
	glBindBuffer(GL_ARRAY_BUFFER, vertexVBO_);
	glBufferSubData(GL_ARRAY_BUFFER, 0,sizeof(float) * numVertices_ * 3, &vertices_[0].x());
	glBufferSubData(GL_ARRAY_BUFFER, sizeof(float) * numVertices_ * 3,sizeof(float) * numVertices_ * 3, &normals_[0].x());
}
//-----------------------------------------------------------------------------
void Water::render(const Camera& cam)
{
	glUseProgram(shader_->handle());

	int loc;
	if ((loc = glGetUniformLocationARB(shader_->handle(), "glow")) >= 0)
		glUniform1iARB(loc, 0);
	if ((loc = glGetUniformLocationARB(shader_->handle(), "color")) >= 0)
		glUniform1iARB(loc, 1);
	if ((loc = glGetUniformLocationARB(shader_->handle(), "stars")) >= 0)
		glUniform1iARB(loc, 2);
	if ((loc = glGetUniformLocationARB(shader_->handle(), "detailWaves")) >= 0)
		glUniform1iARB(loc, 3);

	glActiveTexture( GL_TEXTURE0 );
	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, tex_glow);
	glActiveTexture( GL_TEXTURE1 );
	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, tex_sky);
	glActiveTexture( GL_TEXTURE2 );
	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, tex_stars);
	glActiveTexture( GL_TEXTURE3 );
	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, tex_detailWaves);

	const Eigen::Vector3d sunPos=reflectedSky_->getSunPos();

	if ((loc = glGetUniformLocationARB(shader_->handle(),"lightPos")) >= 0)
		glUniform3fARB(loc, sunPos.x(),sunPos.y(),sunPos.z());

	if ((loc = glGetUniformLocationARB(shader_->handle(),"eyePos")) >= 0)
		glUniform3fARB(loc, cam.getPos().x(),cam.getPos().y(),cam.getPos().z());

	double time=glfwGetTime();

	if ((loc = glGetUniformLocationARB(shader_->handle(),"time")) >= 0)
		glUniform1fARB(loc, time);


	//glBindVertexArray(vertexVBO_);
	glBindBuffer(GL_ARRAY_BUFFER, vertexVBO_);

	glEnableClientState(GL_NORMAL_ARRAY);
	glEnableClientState(GL_VERTEX_ARRAY);
	glVertexPointer(3, GL_FLOAT, 0, NULL);
	glNormalPointer(GL_FLOAT,0,(void*)(sizeof(float) * numVertices_ * 3));

	//glBufferData(GL_ARRAY_BUFFER, sizeof(float) * glui_numVertices * 3, v_p_vertices, GL_STATIC_DRAW);
	//glEnableVertexAttribArray(SHADER_POSITION_LOC);
	//glVertexAttribPointer(SHADER_POSITION_LOC, 3, GL_FLOAT, 0, 0, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexVBO_);

	glDrawElements(GL_TRIANGLES,indices_.size(),GL_UNSIGNED_INT,(void*)0);

	glDisableClientState(GL_VERTEX_ARRAY);
	glDisableClientState(GL_NORMAL_ARRAY);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ARRAY_BUFFER,0);


	glUseProgram(0);

	glActiveTexture( GL_TEXTURE3 );
	glDisable(GL_TEXTURE_2D);
	glActiveTexture( GL_TEXTURE2 );
	glDisable(GL_TEXTURE_2D);
	glActiveTexture( GL_TEXTURE1 );
	glDisable(GL_TEXTURE_2D);
	glActiveTexture( GL_TEXTURE0 );
	glDisable(GL_TEXTURE_2D);
}
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------

