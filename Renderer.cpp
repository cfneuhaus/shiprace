#include "Renderer.h"

#include <GL/glew.h>
#include <GL/glfw.h>

#include <iostream>

namespace
{
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
void my_gluPerspective( GLdouble fovY, GLdouble aspect, GLdouble zNear, GLdouble zFar )
{
	const GLdouble pi = 3.1415926535897932384626433832795;
	GLdouble fW, fH;
	fH = tan( (fovY / 2) / 180 * pi ) * zNear;
	fH = tan( fovY / 360 * pi ) * zNear;
	fW = fH * aspect;
	glFrustum( -fW, fW, -fH, fH, zNear, zFar );
}
void my_gluLookAt(const Eigen::Vector3d& eye, const Eigen::Vector3d& center, const Eigen::Vector3d& up)
{
	const Eigen::Vector3d fw=(center-eye).normalized();

	const Eigen::Vector3d side=fw.cross(up).normalized();

	const Eigen::Vector3d newup=side.cross(fw);

	Eigen::Matrix4d m=Eigen::Matrix4d::Identity();
	m.block<1,3>(0,0)=side;
	m.block<1,3>(1,0)=newup;
	m.block<1,3>(2,0)=-fw;

	Eigen::Matrix4d transf=Eigen::Matrix4d::Identity();
	transf.block<3,1>(0,3)=-eye;
	m=m*transf;

	glMultMatrixd(&m(0,0));
}
}


//-----------------------------------------------------------------------------
Renderer::Renderer(int winWidth, int winHeight) : winWidth_(winWidth), winHeight_(winHeight)
{
	cam_.setPos(Eigen::Vector3d(15,7,8));
	cam_.setLookAt(Eigen::Vector3d(0,0,0));
	cam_.setUp(Eigen::Vector3d(0,0,1));
}
//-----------------------------------------------------------------------------
void Renderer::init()
{
	water_.reset(new GerstnerWater(100,1));

	Eigen::Matrix4d pre=Eigen::Matrix4d::Zero();
	pre(1,0)=1;	pre(0,2)=-1;	pre(2,1)=1;
	pre/=100;	pre(3,3)=1;

	ship_.reset(new BuoyantEntity("Assets/Mesh/black_pearl.obj",pre));
	ship_->setCollisionModel(50,2);

	buoys_.resize(10);
	for (size_t i=0;i<buoys_.size();i++)
	{
		buoys_[i].reset(new BuoyantEntity("Assets/Mesh/cube.obj"));
		buoys_[i]->setPos(Eigen::Vector3d(rand()/float(RAND_MAX)*30-15,rand()/float(RAND_MAX)*30-15,0));
	}

	sky_.reset(new Sky);
}
//-----------------------------------------------------------------------------
void Renderer::processCameraInput(double dt)
{
	static bool firstTime=true;
	if (firstTime)
	{
		glfwSetMousePos(winWidth_/2, winHeight_/2);
		firstTime=false;
	}
	int mouseX=0;
	int mouseY=0;
	glfwGetMousePos(&mouseX, &mouseY);

	const int dx=mouseX-winWidth_/2;
	const int dy=mouseY-winHeight_/2;
	cam_.setViewByMouse(dx, dy);
	glfwSetMousePos(winWidth_/2, winHeight_/2);

	const double Base_Speed=10; // m/s

	double speed=Base_Speed;
	if (glfwGetKey( GLFW_KEY_SPACE ) == GLFW_PRESS)
		speed*=2;

	// Move forward
	if (glfwGetKey( 'W' ) == GLFW_PRESS)
		cam_.moveCamera(speed*dt);
	// Move backward
	if (glfwGetKey( 'S' ) == GLFW_PRESS)
		cam_.moveCamera(-speed*dt);

	// Strafe right
	if (glfwGetKey( 'D' ) == GLFW_PRESS)
		cam_.strafeCamera(speed*dt);
	// Strafe left
	if (glfwGetKey( 'A' ) == GLFW_PRESS)
		cam_.strafeCamera(-speed*dt);
}
//-----------------------------------------------------------------------------
void Renderer::processShipInput(double dt)
{
	if (glfwGetKey( GLFW_KEY_UP ) == GLFW_PRESS)
		ship_->forwards(1.5*dt);
	if (glfwGetKey( GLFW_KEY_DOWN ) == GLFW_PRESS)
		ship_->forwards(-1.5*dt);
	if (glfwGetKey( GLFW_KEY_LEFT ) == GLFW_PRESS)
		ship_->rotate(0.5*dt,Eigen::Vector3d(0,0,1));
	if (glfwGetKey( GLFW_KEY_RIGHT ) == GLFW_PRESS)
		ship_->rotate(-0.5*dt,Eigen::Vector3d(0,0,1));
}
//-----------------------------------------------------------------------------
void Renderer::resolveCollisions()
{
	std::vector<Entity*> ents;
	ents.push_back(ship_.get());
	for (size_t i=0;i<buoys_.size();i++)
		ents.push_back(buoys_[i].get());

	for (size_t i=0;i<ents.size();i++)
	{
		for (size_t j=0;j<ents.size();j++)
		{
			if (i==j)
				continue;

			const double dist=(ents[i]->getPos()-ents[j]->getPos()).norm();

			const double ri=ents[i]->getCollisionRadius();
			const double rj=ents[j]->getCollisionRadius();
			if (dist<ri+rj)
			{
				const double overlap=ri+rj-dist;
				const Eigen::Vector3d dir=(ents[j]->getPos()-ents[i]->getPos()).normalized();

				const double mi=ents[i]->getCollisionMass();
				const double mj=ents[j]->getCollisionMass();
				const double totalMass=mi+mj;

				ents[j]->setPos(ents[j]->getPos()+dir*overlap*mi/totalMass);
				ents[i]->setPos(ents[i]->getPos()-dir*overlap*mj/totalMass);
			}
		}
	}

}
//-----------------------------------------------------------------------------
void Renderer::render()
{
	const double currentTime = glfwGetTime();
	static double lastTime=currentTime-1.0/60.0;
	const double dt = currentTime - lastTime;
	lastTime=currentTime;

	glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);

	processCameraInput(dt);
	processShipInput(dt);

	ship_->swim(*water_.get(),dt);
	for (size_t i=0;i<buoys_.size();i++)
		buoys_[i]->swim(*water_.get(),dt);

	resolveCollisions();


	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	my_gluPerspective(85, float(winWidth_)/float(winHeight_), 0.1, 200.0);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	my_gluLookAt(cam_.getPos(), cam_.getLookAt(), cam_.getUp());

	sky_->update(dt);
	sky_->render();


	glColor3f(1,1,1);
	render_coordinateAxes(1);
	glColor3f(1,1,1);
	//glPolygonMode(GL_FRONT_AND_BACK,GL_LINE);
	water_->update(dt);
	water_->render();
	//glPolygonMode(GL_FRONT_AND_BACK,GL_FILL);

	ship_->render();

	for (size_t i=0;i<buoys_.size();i++)
		buoys_[i]->render();
}
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
