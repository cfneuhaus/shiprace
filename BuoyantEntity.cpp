#include "BuoyantEntity.h"
#include "Water.h"

#include <iostream>

void render_coordinateAxes(double size)
{
	glEnable(GL_LINE_SMOOTH);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	glLineWidth(2.5);
	glColor3f(0, 1, 0);
	glBegin(GL_LINES);
	glVertex3f(0, 0, 0);
	glVertex3f(0, size, 0);
	glColor3f(1, 0, 0);
	glVertex3f(0, 0, 0);
	glVertex3f(size, 0, 0);
	glColor3f(0, 0, 1);
	glVertex3f(0, 0, 0);
	glVertex3f(0, 0, size);
	glEnd();
	glLineWidth(1);

	glDisable(GL_BLEND);
	glDisable(GL_LINE_SMOOTH);
}

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

	Eigen::Matrix4d tmp=transform_*bobbingTransform_*preTransform_;
	Eigen::Matrix4d invT=tmp.inverse();
	Eigen::Vector4d sunW;
	sunW<< sunPos_,0.0;
	sunW=invT*sunW;
	int loc;
	if ((loc = glGetUniformLocationARB(shader_->handle(),"lightPos")) >= 0)
		glUniform3fARB(loc, sunW.x(),sunW.y(),sunW.z());


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
