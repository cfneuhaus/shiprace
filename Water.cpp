#include "Water.h"

#include <GL/glew.h>
#include <GL/glfw.h>

//-----------------------------------------------------------------------------
Water::Water(int surfaceSize, double sizePerCell) : surfaceSize_(surfaceSize), sizePerCell_(sizePerCell)
{
	numVertices_=surfaceSize_*surfaceSize_;
	numFaces_=(surfaceSize_-1)*(surfaceSize_-1)*2;

	initGeometry();

//	if (glb_normals == true) initiateNormals();
//	if (glb_texCoord == true) initiateTextureCoodrinates();
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

	// Generiere Vertices einer planaren Wasseroberfläche in der x-y-Ebene
	{
		int i=0;
		for(int y=(-1)*(surfaceSize_ / 2) + 1; y <= (surfaceSize_ / 2); y++)
			for (int x=(-1)*(surfaceSize_ / 2) + 1; x <= (surfaceSize_ / 2); x++)
				vertices_[i++] << x * sizePerCell_, y * sizePerCell_, 0;

		glGenBuffers(1, &vertexVBO_);
		glBindBuffer(GL_ARRAY_BUFFER, vertexVBO_);
		glBufferData(GL_ARRAY_BUFFER, sizeof(float) * numVertices_ * 3, &vertices_[0].x(), GL_STATIC_DRAW);
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
	glBufferData(GL_ARRAY_BUFFER, sizeof(float) * numVertices_ * 3, &vertices_[0].x(), GL_STATIC_DRAW);
}
//-----------------------------------------------------------------------------
void Water::render()
{
	//glBindVertexArray(vertexVBO_);
	glBindBuffer(GL_ARRAY_BUFFER, vertexVBO_);

	glEnableClientState(GL_VERTEX_ARRAY);
	glVertexPointer(3, GL_FLOAT, sizeof(GLfloat)*3, NULL);

	//glBufferData(GL_ARRAY_BUFFER, sizeof(float) * glui_numVertices * 3, v_p_vertices, GL_STATIC_DRAW);
	//glEnableVertexAttribArray(SHADER_POSITION_LOC);
	//glVertexAttribPointer(SHADER_POSITION_LOC, 3, GL_FLOAT, 0, 0, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexVBO_);

	glDrawElements(GL_TRIANGLES,indices_.size(),GL_UNSIGNED_INT,(void*)0);

	glDisableClientState(GL_VERTEX_ARRAY);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ARRAY_BUFFER,0);
}
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------

