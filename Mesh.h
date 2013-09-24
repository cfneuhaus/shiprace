#ifndef MESH_H
#define	MESH_H

#include <Eigen/Dense>
#include <map>
#include <vector>

#include <GL/glew.h>

class aiScene;
class aiMesh;


struct Vertex
{
	Eigen::Matrix<float,3,1,Eigen::DontAlign> m_pos;
	Eigen::Matrix<float,2,1,Eigen::DontAlign> m_tex;
	Eigen::Matrix<float,3,1,Eigen::DontAlign> m_normal;

	Vertex() {}

	Vertex(const Eigen::Vector3f& pos, const Eigen::Vector2f& tex, const Eigen::Vector3f& normal)
	{
		m_pos    = pos;
		m_tex    = tex;
		m_normal = normal;
	}
};


class Mesh
{
public:
	Mesh();
	Mesh(const Mesh&) = delete;
	Mesh& operator=(const Mesh&) = delete;

	~Mesh();

	bool LoadMesh(const std::string& Filename);

	void Render();

private:
	bool InitFromScene(const aiScene* pScene, const std::string& Filename);
	void InitMesh(unsigned int Index, const aiMesh* paiMesh);
	bool InitMaterials(const aiScene* pScene, const std::string& Filename);
	void Clear();

#define INVALID_MATERIAL 0xFFFFFFFF

	struct MeshEntry {
		MeshEntry();

		~MeshEntry();

		void Init(const std::vector<Vertex>& Vertices,
				  const std::vector<unsigned int>& Indices);

		GLuint VB;
		GLuint IB;
		unsigned int NumIndices;
		unsigned int MaterialIndex;
	};

	std::vector<MeshEntry> m_Entries;
	std::vector<Eigen::Matrix<double,3,1,Eigen::DontAlign> > m_colors;
};

#endif


