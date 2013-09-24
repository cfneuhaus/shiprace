#include "Camera.h"

using namespace Eigen;

//-----------------------------------------------------------------------------
Camera::Camera()
{
	m_sensitivity=1.0f/170.0f;

	m_up << 0.0f,0.0f,1.0f;
	m_lookAt << 0.0f,1.0f,0.0f;
	m_pos << 0.0f,0.0f,0.0f;
}
//-----------------------------------------------------------------------------
Camera::~Camera()
{
}
//-----------------------------------------------------------------------------
void Camera::setMouseSensitivity(double sens)
{
	m_sensitivity=sens;
}
//-----------------------------------------------------------------------------
void Camera::strafeCamera(double dist)
{
	Vector3d right=(m_lookAt-m_pos).cross(m_up).normalized();
	m_pos+=right*dist*(2.0f/3.0f);
	m_lookAt+=right*dist*(2.0f/3.0f);
}
//-----------------------------------------------------------------------------
void Camera::truckCamera(double dist)
{
	Vector3d front=(m_lookAt-m_pos);
	front.z()=0;
	front.normalize();
	m_pos+=front*dist*(2.0f/3.0f);
	m_lookAt+=front*dist*(2.0f/3.0f);
}
//-----------------------------------------------------------------------------
void Camera::moveCamera(double dist)
{
	Vector3d view=(m_lookAt-m_pos).normalized();
	m_pos+=view*dist;
	m_lookAt+=view*dist;
}
//-----------------------------------------------------------------------------
void Camera::setViewByMouse(int dx, int dy)
{
	if ((dx==0)&&(dy==0))
		return;

    double currentRotX=M_PI*0.5-acos(m_up.normalized().dot((m_lookAt-m_pos).normalized()));

    double angleY=double(-dx)*m_sensitivity;
    double angleZ=double(-dy)*m_sensitivity;

    const double piHalf=M_PI*0.5-0.03125;

	
	if (currentRotX+angleZ>piHalf)
		angleZ=piHalf-currentRotX;
	else if (currentRotX+angleZ<-piHalf)
		angleZ=-piHalf-currentRotX;

	Vector3d axis=(m_lookAt-m_pos).cross(m_up).normalized();

	rotateView(Vector3d(axis.x(),axis.y(),0),angleZ);
	rotateView(Vector3d(0,0,1),angleY);
}
//-----------------------------------------------------------------------------
void Camera::rotateView(const Vector3d& axis, double angle)
{
    AngleAxis<double> t=AngleAxis<double>(angle,axis.normalized());

    //Mat3f rot=rotQuat<double>(axis.getNormalized(),angle).toMat3();
	m_lookAt=m_pos+t*(m_lookAt-m_pos).normalized();
}
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
void Camera::setPos(const Vector3d& pos)
{
	m_pos=pos;
}
//-----------------------------------------------------------------------------
void Camera::setLookAt(const Vector3d& lookAt)
{
	m_lookAt=lookAt;
}
//-----------------------------------------------------------------------------
void Camera::setUp(const Vector3d& up)
{
	m_up=up;
}
//-----------------------------------------------------------------------------
const Vector3d& Camera::getPos() const
{
	return m_pos;
}
//-----------------------------------------------------------------------------
const Vector3d& Camera::getLookAt() const
{
	return m_lookAt;
}
//-----------------------------------------------------------------------------
const Vector3d& Camera::getUp() const
{
	return m_up;
}
//-----------------------------------------------------------------------------
double Camera::getSensitivity() const
{
	return m_sensitivity;
}
//-----------------------------------------------------------------------------
Quaterniond Camera::getQuat() const
{
	Vector3d f=(m_lookAt-m_pos).normalized();
	Vector3d s=f.cross(m_up);
	Vector3d u=s.cross(f);

	Matrix3d m;
	m.row(0)=s;
	m.row(1)=u;
	m.row(2)=-f;

    return Quaterniond(m);
}
//-----------------------------------------------------------------------------




