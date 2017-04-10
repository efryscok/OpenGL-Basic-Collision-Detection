#include "cMeshTypeManager.h"

std::map<std::string, std::vector<cPlyTriangle>> m_mapMeshNameToTriInfo;

bool cMeshTypeManager::m_generateTriangleInfoFromIndexedVectors(std::string filename, std::vector<cPlyVertexXYZ> &vecPlyVerts, std::vector<cPlyTriFace> &vecPlyIndices) {
	std::vector< cPlyTriangle > vecTriangles;

	vecTriangles.reserve(vecPlyIndices.size());

	int numberOfTriangles = vecPlyIndices.size();
	cPlyTriangle tempTri;
	for (int count = 0; count != numberOfTriangles; count++) {
		vecTriangles.push_back(tempTri);
	}

	for (int index = 0; index != numberOfTriangles; index++) {
		int triVert1_X_index = vecPlyIndices[index].v1;
		float vertex1_x_value = vecPlyVerts[triVert1_X_index].x;
		vecTriangles[index].v1.x = vertex1_x_value;

		vecTriangles[index].v1.x = vecPlyVerts[vecPlyIndices[index].v1].x;

		vecTriangles[index].v1.y = vecPlyVerts[vecPlyIndices[index].v1].y;
		vecTriangles[index].v1.z = vecPlyVerts[vecPlyIndices[index].v1].z;

		vecTriangles[index].v2.x = vecPlyVerts[vecPlyIndices[index].v2].x;
		vecTriangles[index].v2.y = vecPlyVerts[vecPlyIndices[index].v2].y;
		vecTriangles[index].v2.z = vecPlyVerts[vecPlyIndices[index].v2].z;

		vecTriangles[index].v3.x = vecPlyVerts[vecPlyIndices[index].v3].x;
		vecTriangles[index].v3.y = vecPlyVerts[vecPlyIndices[index].v3].y;
		vecTriangles[index].v3.z = vecPlyVerts[vecPlyIndices[index].v3].z;

	}

	this->m_MapMeshNameToTriInfo[filename] = vecTriangles;

	return true;
}

bool cMeshTypeManager::GetTriangleVertexInformation(std::string meshname, std::vector< cPlyTriangle > &vecTris) {
	std::map< std::string, std::vector< cPlyTriangle > >::iterator itMeshTri = this->m_MapMeshNameToTriInfo.find(meshname);

	if (itMeshTri == this->m_MapMeshNameToTriInfo.end()) {
		return false;
	}

	vecTris = itMeshTri->second;

	return true;
}
