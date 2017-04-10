#include <glad/glad.h>

#include <vector>
#include <fstream>

#include "cMeshTypeManager.h"
#include "vert_XYZ_RGB.h"

bool cMeshTypeManager::LoadPlyFileIntoGLBuffer(unsigned int programID, std::string plyFile, bool saveTriInfo) {
	std::vector< cPlyVertexXYZ > vecPlyVerts;
	std::vector< cPlyTriFace > vecPlyIndices;

	if (!this->m_loadPlyModel(plyFile, vecPlyVerts, vecPlyIndices)) {
		return false;
	}

	// Calculate the maximum extent (max-min) for x,y,z
	float minX, minY, minZ;
	float maxX, maxY, maxZ;

	minX = maxX = vecPlyVerts[0].x;
	minY = maxY = vecPlyVerts[0].y;
	minZ = maxZ = vecPlyVerts[0].z;

	for (int index = 0; index != vecPlyVerts.size(); index++) {
		if (vecPlyVerts[index].x < minX) { minX = vecPlyVerts[index].x; }
		if (vecPlyVerts[index].y < minY) { minY = vecPlyVerts[index].y; }
		if (vecPlyVerts[index].z < minZ) { minZ = vecPlyVerts[index].z; }

		if (vecPlyVerts[index].x > maxX) { maxX = vecPlyVerts[index].x; }
		if (vecPlyVerts[index].y > maxY) { maxY = vecPlyVerts[index].y; }
		if (vecPlyVerts[index].z > maxZ) { maxZ = vecPlyVerts[index].z; }
	}

	// What the max extent
	float extentX = maxX - minX;
	float extentY = maxY - minY;
	float extentZ = maxZ - minZ;

	float maxExtent = extentX;
	if (extentY > maxExtent) { maxExtent = extentY; }
	if (extentZ > maxExtent) { maxExtent = extentZ; }

	int numberofVerts = vecPlyVerts.size();
	vert_XYZ_RGB* p_vertArray = new vert_XYZ_RGB[numberofVerts];

	for (int index = 0; index != numberofVerts; index++) {
		p_vertArray[index].x = vecPlyVerts[index].x;
		p_vertArray[index].y = vecPlyVerts[index].y;
		p_vertArray[index].z = vecPlyVerts[index].z;

		p_vertArray[index].r = 1.0f;
		p_vertArray[index].g = 1.0f;
		p_vertArray[index].b = 1.0f;
	}

	cMeshTypeManager::VAOInfo tempMeshVAOInfo;
	tempMeshVAOInfo.meshName = plyFile;
	tempMeshVAOInfo.unitScaleValue = 1.0f / maxExtent;

	glGenVertexArrays(1, &(tempMeshVAOInfo.VAO_ID));
	glBindVertexArray(tempMeshVAOInfo.VAO_ID);

	GLuint vertex_buffer = 0;
	GLuint index_buffer = 0;

	glGenBuffers(1, &vertex_buffer);
	glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer);

	int sizeInBytes = sizeof(vert_XYZ_RGB) * numberofVerts;

	glBufferData(GL_ARRAY_BUFFER, sizeInBytes, p_vertArray, GL_STATIC_DRAW);

	glGenBuffers(1, &index_buffer);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, index_buffer);

	int numberOfTriangles = vecPlyIndices.size();
	tempMeshVAOInfo.numberOfIndices = numberOfTriangles * 3;
	int* pIndexArray = new int[tempMeshVAOInfo.numberOfIndices];

	int indexBufferIndex = 0;
	for (int triIndex = 0; triIndex != vecPlyIndices.size(); triIndex++, indexBufferIndex += 3) {
		pIndexArray[indexBufferIndex + 0] = vecPlyIndices[triIndex].v1;
		pIndexArray[indexBufferIndex + 1] = vecPlyIndices[triIndex].v2;
		pIndexArray[indexBufferIndex + 2] = vecPlyIndices[triIndex].v3;
	}

	int indexBufferArraySizeInBytes = tempMeshVAOInfo.numberOfIndices * sizeof(int);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, indexBufferArraySizeInBytes, pIndexArray, GL_STATIC_DRAW);

	int sizeOfVertInBytes = sizeof(vert_XYZ_RGB);

	GLuint vpos_location = glGetAttribLocation(programID, "vPosition");	// 8
	GLuint vcol_location = glGetAttribLocation(programID, "vColour");	// 24

	glEnableVertexAttribArray(vpos_location);
	glVertexAttribPointer(vpos_location, 3, GL_FLOAT, GL_FALSE, sizeOfVertInBytes, (void*)0);

	int offsetInBytesToColour = offsetof(vert_XYZ_RGB, r);
	glEnableVertexAttribArray(vcol_location);
	glVertexAttribPointer(vcol_location, 3, GL_FLOAT, GL_FALSE, sizeOfVertInBytes, (void*)(sizeof(float) * 3));

	delete[] p_vertArray;
	delete[] pIndexArray;

	this->m_MapMeshNameToVAOInfo[plyFile] = tempMeshVAOInfo;

	glBindVertexArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

	if (saveTriInfo) {
		this->m_generateTriangleInfoFromIndexedVectors(plyFile, vecPlyVerts, vecPlyIndices);
	}

	return true;
}

bool cMeshTypeManager::LookUpMeshInfo(std::string meshName, unsigned int &VAO_ID, int &numberOfIndices, float &unitScale) {
	std::map< std::string, cMeshTypeManager::VAOInfo >::iterator itVAO = this->m_MapMeshNameToVAOInfo.find(meshName);

	if (itVAO == this->m_MapMeshNameToVAOInfo.end()) {
		return false;
	}

	VAO_ID = itVAO->second.VAO_ID;
	numberOfIndices = itVAO->second.numberOfIndices;
	unitScale = itVAO->second.unitScaleValue;

	return true;
}

bool cMeshTypeManager::m_loadPlyModel(std::string filename, std::vector< cPlyVertexXYZ > &vecPlyVerts, std::vector< cPlyTriFace > &vecPlyIndices) {
	std::ifstream theFile(filename.c_str());
	if (!theFile.is_open()) {
		return false;
	}

	std::string temp;
	bool bKeepReading = true;
	while (bKeepReading) {
		theFile >> temp;

		if (temp == "vertex") {
			bKeepReading = false;
		}
	}

	int numVertices = 0;
	theFile >> numVertices;

	bKeepReading = true;
	while (bKeepReading) {
		theFile >> temp;

		if (temp == "face") {
			bKeepReading = false;
		}
	}

	int numberOfTriangles = 0;
	theFile >> numberOfTriangles;

	bKeepReading = true;
	while (bKeepReading) {
		theFile >> temp;

		if (temp == "end_header") {
			bKeepReading = false;
		}
	}

	for (int index = 0; index != numVertices; index++) {
		cPlyVertexXYZ tempVert;
		theFile >> tempVert.x >> tempVert.y >> tempVert.z;

		vecPlyVerts.push_back(tempVert);
	}

	for (int index = 0; index != numberOfTriangles; index++) {
		cPlyTriFace tempTri;
		int numIndicesDiscard = 0;
		theFile >> numIndicesDiscard >> tempTri.v1 >> tempTri.v2 >> tempTri.v3;
		vecPlyIndices.push_back(tempTri);
	}

	return true;
}