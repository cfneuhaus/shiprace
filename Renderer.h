#ifndef RENDERER_H
#define RENDERER_H

#include <memory>
#include "Camera.h"
#include "GerstnerWater.h"
#include "Mesh.h"
#include "Entity.h"
#include "BuoyantEntity.h"
#include "Sky.h"

class Renderer
{
public:
	Renderer(int winWidth, int winHeight);
	void init();
	void render();
private:
	void doPhysics(double dt);

	int winWidth_;
	int winHeight_;

	void processCameraInput(double dt);
	void processShipInput(double dt);

	void resolveCollisions();


	Camera cam_;

	std::unique_ptr<GerstnerWater> water_;
	//std::unique_ptr<Mesh> test_;
	std::vector<std::unique_ptr<BuoyantEntity> > buoys_;

	std::unique_ptr<BuoyantEntity> ship_;

	std::unique_ptr<Sky> sky_;


};

#endif
