#ifndef BUOYANTENTITY_H
#define BYOYANTENTITY_H

#include <string>
#include <Eigen/Core>
#include "Entity.h"

class Water;

class BuoyantEntity : public Entity
{
public:
	BuoyantEntity(const std::string& objectName, const Eigen::Matrix4d& preTransform=Eigen::Matrix4d::Identity());

	void swim(const Water& water, double dt);
	virtual void render();

	Eigen::Matrix<double,4,4,Eigen::DontAlign> bobbingTransform_;
	double animTime_;

	double randomAnimOffset_;
};

#endif
