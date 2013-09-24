#ifndef ENTITY_H
#define ENTITY_H

#include <memory>
#include <string>
#include <Eigen/Core>
#include "Mesh.h"
#include "Shader.h"

class Entity
{
public:
	Entity();
	Entity(const std::string& objectName, const Eigen::Matrix4d& preTransform=Eigen::Matrix4d::Identity());

	void setCollisionModel(double mass, double radius);
	double getCollisionMass() const { return coll_mass_; }
	double getCollisionRadius() const { return coll_radius_; }

	virtual void render();

	Eigen::Vector3d getPos() const { return transform_.block<3,1>(0,3); }
	void setPos(const Eigen::Vector3d& pos) { transform_.block<3,1>(0,3)=pos; }

	Eigen::Matrix<double,4,4,Eigen::DontAlign>& transform() { return transform_; }
	const Eigen::Matrix<double,4,4,Eigen::DontAlign>& transform() const { return transform_; }

	void forwards(double amount);
	void rotate(double angle, const Eigen::Vector3d& axis);

protected:
	double coll_mass_;
	double coll_radius_;

	Eigen::Matrix<double,4,4,Eigen::DontAlign> preTransform_;

	Eigen::Matrix<double,4,4,Eigen::DontAlign> transform_;

	std::unique_ptr<Mesh> model_;

	std::unique_ptr<Shader> shader_;

};

#endif
