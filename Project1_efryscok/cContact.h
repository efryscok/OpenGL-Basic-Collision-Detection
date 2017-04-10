#pragma once

#include <glm/glm.hpp>
#include "cGameObject.h"

class cContact {
public:
	enum eCollisionType {
		WITH_SPHERE,
		WITH_TRIANGLE,
		NO_COLLISION
	};

	unsigned int objectID;
	float penetrationDepth;

	glm::vec3 contactXYZ;
	glm::vec3 normal;

	eCollisionType collisionType;

public:
	cContact() : penetrationDepth(0.f), objectID(0), collisionType(NO_COLLISION) {};
};