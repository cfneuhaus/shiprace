#include "BuoyantEntity.h"
#include "Water.h"

//-----------------------------------------------------------------------------
BuoyantEntity::BuoyantEntity(const std::string& objectName, const Eigen::Matrix4d& preTransform)
	: Entity(objectName,preTransform)
{
	bobbingTransform_=Eigen::Matrix4d::Identity();
	animTime_=0;
}
//-----------------------------------------------------------------------------
void BuoyantEntity::swim(const Water& water, double dt)
{
	transform_(2,3)=transform_(2,3)*0.95+(1.0-0.95)*water.computeHeightAtXY(transform_.block<2,1>(0,3));

	animTime_+=dt;
	Eigen::AngleAxisd xbob(sin(animTime_*0.1)*0.1,Eigen::Vector3d(1,0,0));
	Eigen::AngleAxisd ybob(cos(animTime_*0.07+0.3)*0.07,Eigen::Vector3d(0,1,0));
	bobbingTransform_.block<3,3>(0,0)=xbob.toRotationMatrix()*ybob.toRotationMatrix();
}
//-----------------------------------------------------------------------------
void BuoyantEntity::render()
{
	glUseProgram(shader_->handle());

	Eigen::Vector3d sunPos_;
	sunPos_ << 0.1,0,0.9;
	int loc;
	if ((loc = glGetUniformLocationARB(shader_->handle(),"lightPos")) >= 0)
		glUniform3fARB(loc, sunPos_.x(),sunPos_.y(),sunPos_.z());

	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();
	glMultMatrixd(&transform_(0,0));
	glMultMatrixd(&bobbingTransform_(0,0));
	glMultMatrixd(&preTransform_(0,0));
	model_->Render();
	glPopMatrix();

	glUseProgram(0);
}
//-----------------------------------------------------------------------------
