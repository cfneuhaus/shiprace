#ifndef WATER_H
#define WATER_H

#include <Eigen/Core>
#include <vector>

class Water
{
public:
	Water(int surfaceSize, double sizePerCell);
	void render();
	virtual void update(double dt);
	virtual double computeHeightAtXY(const Eigen::Vector2d& pos) const;
protected:
	void updateGeometry();

	void initGeometry();

	double sizePerCell_;
	int surfaceSize_;
	int numVertices_;
	int numFaces_;
	std::vector<Eigen::Matrix<float,3,1,Eigen::DontAlign> > vertices_;
	std::vector<int> indices_;

	unsigned int vertexVBO_;
	unsigned int indexVBO_;

};

#endif
