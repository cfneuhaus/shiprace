#ifndef CAMERA_H
#define CAMERA_H

#include <Eigen/Core>
#include <Eigen/Geometry>

class Camera
{
public:
	Camera();
	~Camera();

	//! Strafes the camera sidewards by the specified amount
    void strafeCamera(double dist);
	//! Trucks the camera z-plane for-/backwards by specified amount parallel to z-plane
    void truckCamera(double dist);
	//! Moves the camera for-/backwards by the specified amount
    void moveCamera(double dist);

	//! Modifies the mouse sensitivity. Default: 1.0/170.0
    void setMouseSensitivity(double sens);

	//! Rotates the current view given a delta mouse movement
	void setViewByMouse(int dx, int dy);

	void setPos(const Eigen::Vector3d& pos);
	//! Set LookAt Point
	void setLookAt(const Eigen::Vector3d& lookAt);
	void setUp(const Eigen::Vector3d& up);

	const Eigen::Vector3d& getPos() const;
	//! Get LookAt Point
	const Eigen::Vector3d& getLookAt() const;
	const Eigen::Vector3d& getUp() const;

	Eigen::Quaterniond getQuat() const;

	//! Get mouse sensitivity
    double getSensitivity() const;
private:
	void rotateView(const Eigen::Vector3d& axis, double angle);
    double m_sensitivity;
	Eigen::Vector3d m_pos;
	Eigen::Vector3d m_lookAt;
	Eigen::Vector3d m_up;
};

#endif

