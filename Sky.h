#ifndef SKY_H
#define SKY_H

#include "Entity.h"
#include "Shader.h"

class Sky : public Entity
{
public:
	Sky();

	virtual void render();

	void update(double dt);

	void setTimeOfDay(double t);
	void setSecondsPerHour(double secPerH);

	Eigen::Vector3d getSunPos() const { return sunPos_; }
private:
	std::unique_ptr<Shader> shader_;

	GLuint tex_glow;
	GLuint tex_sky;

	GLuint tex_stars;

	double timeOfDay_;
	double secPerH_;
	Eigen::Matrix<double,3,1,Eigen::DontAlign> sunPos_;
};


#endif
