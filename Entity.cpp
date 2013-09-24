#include "Entity.h"

#include <GL/glew.h>
#include <GL/glfw.h>

#include <iostream>

//-----------------------------------------------------------------------------
Entity::Entity(const std::string& objectName, const Eigen::Matrix4d& preTransform)
	: preTransform_(preTransform),
	  coll_mass_(1),
	  coll_radius_(1)
{
	transform_=Eigen::Matrix4d::Identity();

	model_.reset(new Mesh);
	model_->LoadMesh(objectName);
}
//-----------------------------------------------------------------------------
void Entity::setCollisionModel(double mass, double radius)
{
	coll_mass_=mass;
	coll_radius_=radius;
}
//-----------------------------------------------------------------------------
void Entity::forwards(double amount)
{
	const Eigen::Vector3d xaxis=transform_.block<3,1>(0,0);
	transform_.block<3,1>(0,3)+=xaxis*amount;
}
//-----------------------------------------------------------------------------
void Entity::rotate(double angle, const Eigen::Vector3d& axis)
{
	Eigen::AngleAxisd aa(angle,axis);
	transform_.block<3,3>(0,0)=aa.toRotationMatrix()*transform_.block<3,3>(0,0);
}
//-----------------------------------------------------------------------------
void Entity::render()
{
	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();
	glMultMatrixd(&transform_(0,0));
	glMultMatrixd(&preTransform_(0,0));
	model_->Render();
	glPopMatrix();
}
//-----------------------------------------------------------------------------|
