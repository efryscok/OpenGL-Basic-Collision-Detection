#pragma once

#include <string>

// General class used to represent an object to render
class cGameObject {
	// Data
public:
	// Flag variables
	bool bIsUpdatedByCollision;
	bool bIsUpdatedByPhysics;
	bool bIsWireframe;
	bool bIsControlledByUser;

	// Physics variables
	float Vx, Vy, Vz;
	float Ax, Ay, Az;
	float pre_Rot_X, pre_Rot_Y, pre_Rot_Z;
	float post_Rot_X, post_Rot_Y, post_Rot_Z;

	// General object variables
	float x, y, z;
	float scale;
	float radius;
	float lastX, lastY, lastZ;
	float solid_R, solid_G, solid_B;
	float groundCollisionFadeCounter;
	float objectCollisionFadeCounter;

	// Mesh info
	int meshID;
	std::string meshName;

private:
	int m_uniqueID;
	static int m_nextID;

	// Functions
public:
	cGameObject();

	int getID();
};
