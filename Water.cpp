#include "Water.h"
#include <Eigen/Geometry>

#include <GL/glew.h>
#include <GL/glfw.h>

//-----------------------------------------------------------------------------
Water::Water(int surfaceSize, double sizePerCell) : surfaceSize_(surfaceSize), sizePerCell_(sizePerCell)
{
	numVertices_=surfaceSize_*surfaceSize_;
	numFaces_=(surfaceSize_-1)*(surfaceSize_-1)*2;

	initGeometry();

	shader_.reset(new Shader("Shader/water.vert","Shader/water.frag"));
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

		normals_[i/3]+=(b-a).cross(c-a);;
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
void Water::render()
{
	glUseProgram(shader_->handle());

	//glBindVertexArray(vertexVBO_);
	glBindBuffer(GL_ARRAY_BUFFER, vertexVBO_);

	glEnableClientState(GL_NORMAL_ARRAY);
	glEnableClientState(GL_VERTEX_ARRAY);
	glVertexPointer(3, GL_FLOAT, sizeof(GLfloat)*3, NULL);
	glNormalPointer(GL_FLOAT,sizeof(GLfloat)*3,(void*)(numVertices_ * 3));

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

}
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------

