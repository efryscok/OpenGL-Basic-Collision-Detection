#pragma once

#include <glm/glm.hpp>
#include <string>
#include <vector>
#include <map>

// Used to store the vertex locations read from ply file
struct cPlyVertexXYZ {
	cPlyVertexXYZ() : x(0.0f), y(0.0f), z(0.0f) {}
	float x;
	float y;
	float z;
};

// Used to store the indice numbers read from ply file
struct cPlyTriFace {
	cPlyTriFace() : v1(-1), v2(-1), v3(-1) {}
	int v1;
	int v2;
	int v3;
};

struct cPlyTriangle {
	glm::vec3 v1;
	glm::vec3 v2;
	glm::vec3 v3;
};

// Class for managing mesh information for many meshs
class cMeshTypeManager {
	// Data
private:
	struct VAOInfo {
		VAOInfo() : VAO_ID(0), numberOfIndices(0), unitScaleValue(1.0f) {}
		std::string meshName;
		unsigned int VAO_ID;
		unsigned int numberOfIndices;
		float unitScaleValue;
	};

	std::map<std::string, VAOInfo> m_MapMeshNameToVAOInfo;
	std::map<std::string, std::vector<cPlyTriangle>> m_MapMeshNameToTriInfo;

	// Functions
public:
	bool LoadPlyFileIntoGLBuffer(unsigned int programID, std::string plyFile, bool saveTriInfo);
	bool LookUpMeshInfo(std::string meshName, unsigned int &VAO_ID, int &numberOfIndices, float &unitScale);
	bool GetTriangleVertexInformation(std::string meshname, std::vector<cPlyTriangle> &vecTris);

private:
	bool m_loadPlyModel(std::string filename, std::vector< cPlyVertexXYZ > &vecPlyVerts, std::vector< cPlyTriFace > &vecPlyIndices);
	bool m_generateTriangleInfoFromIndexedVectors(std::string filename, std::vector<cPlyVertexXYZ> &vecPlyVerts, std::vector<cPlyTriFace> &vecPlyIndices);
};
