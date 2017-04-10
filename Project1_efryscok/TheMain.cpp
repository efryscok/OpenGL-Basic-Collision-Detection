#define _CRTDBG_MAP_ALLOC
#include <cstdlib>
#include <crtdbg.h>

#include <glad/glad.h>		
#include <GLFW/glfw3.h>		
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/rotate_vector.hpp>

#include <iostream>
#include <stdlib.h>
#include <stdio.h>
#include <sstream>
#include <vector>
#include <map>

#include "cCamera.h"
#include "cContact.h"
#include "cGameObject.h"
#include "cMeshTypeManager.h"
#include "cShaderManager.h"
#include "vert_XYZ_RGB.h"

// Redefine the new operator for memory leak checking
#ifdef _DEBUG
#ifndef DBG_NEW
#define DBG_NEW new ( _NORMAL_BLOCK , __FILE__ , __LINE__ )
#define new DBG_NEW 
#endif
#endif

// OpenGL related variables
GLFWwindow* window;
GLint mvp_location;
GLuint shadProgID;
glm::mat4x4 p, v;
GLuint UniformLoc_ID_objectColour = 0;
GLuint UniformLoc_ID_isWireframe = 0;

// Custom objects
std::map<int, std::string> g_map_fileGOs;
std::vector<cGameObject*> g_vec_pGOs;
std::vector<cContact> g_vec_Contacts;
cMeshTypeManager* g_pTheMeshTypeManager = 0;
cShaderManager* g_pTheShaderManager = 0;
cCamera g_Camera(
	0.6f, 1200.f / 800.f, 0.01f, 1000.f,

	glm::vec3(0.f, 200.f, 250.f),
	glm::vec3(0.0f, 25.0f, 0.0f),
	glm::vec3(0.0f, 1.0f, 0.0f)
);

// Method declarations
glm::vec3 ClosestPtPointTriangle(glm::vec3 p, glm::vec3 a, glm::vec3 b, glm::vec3 c);
void CollisionStep(float deltaTime);
void CreateObject(cGameObject* object, std::string description);
void DrawScene();
cGameObject* FindObjectByID(unsigned int ID);
void ImportScene();
void InitWindow();
void PhysicsStep(float deltaTime);
bool TestSphereSphereCollision(cGameObject* pBallA, cGameObject* pBallB);

// Callback functions
static void error_callback(int error, const char* description) {
	fprintf(stderr, "Error: %s\n", description);

	return;
}

static void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods) {
	float MovementSpeed = 1.f;

	if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS) {
		glfwSetWindowShouldClose(window, GLFW_TRUE);
	}

	switch (key) {
	/*
	CONTROLS FOR GAME OBJECTS

	W - FORWARD
	A - LEFT
	S - BACKWARD
	D - RIGHT
	Q - DOWN
	E - UP
	*/
	//case GLFW_KEY_A:
	//	for (std::vector<cGameObject*>::iterator itGO = ::g_vec_pGOs.begin(); itGO != ::g_vec_pGOs.end(); ++itGO) {
	//		if ((*itGO)->bIsControlledByUser) {
	//			(*itGO)->Ax -= MovementSpeed;
	//		}
	//	}
	//	break;
	//case GLFW_KEY_D:
	//	for (std::vector<cGameObject*>::iterator itGO = ::g_vec_pGOs.begin(); itGO != ::g_vec_pGOs.end(); ++itGO) {
	//		if ((*itGO)->bIsControlledByUser) {
	//			(*itGO)->Ax += MovementSpeed;
	//		}
	//	}
	//	break;
	//case GLFW_KEY_W:
	//	for (std::vector<cGameObject*>::iterator itGO = ::g_vec_pGOs.begin(); itGO != ::g_vec_pGOs.end(); ++itGO) {
	//		if ((*itGO)->bIsControlledByUser) {
	//			(*itGO)->Az -= MovementSpeed;
	//		}
	//	}
	//	break;
	//case GLFW_KEY_S:
	//	for (std::vector<cGameObject*>::iterator itGO = ::g_vec_pGOs.begin(); itGO != ::g_vec_pGOs.end(); ++itGO) {
	//		if ((*itGO)->bIsControlledByUser) {
	//			(*itGO)->Az += MovementSpeed;
	//		}
	//	}
	//	break;
	//case GLFW_KEY_Q:
	//	for (std::vector<cGameObject*>::iterator itGO = ::g_vec_pGOs.begin(); itGO != ::g_vec_pGOs.end(); ++itGO) {
	//		if ((*itGO)->bIsControlledByUser) {
	//			(*itGO)->Ay -= MovementSpeed;
	//		}
	//	}
	//	break;
	//case GLFW_KEY_E:
	//	for (std::vector<cGameObject*>::iterator itGO = ::g_vec_pGOs.begin(); itGO != ::g_vec_pGOs.end(); ++itGO) {
	//		if ((*itGO)->bIsControlledByUser) {
	//			(*itGO)->Ay += MovementSpeed;
	//		}
	//	}
	//	break;
	/*
	CONTROLS FOR CAMERA

	UP - FORWARD
	LEFT - LEFT
	DOWN - BACKWARD
	RIGHT - RIGHT
	RIGHT CONTROL - DOWN
	KEY PAD 0 - UP
	*/
	case GLFW_KEY_W:
		g_Camera.eye.z -= MovementSpeed;
		break;
	case GLFW_KEY_S:
		g_Camera.eye.z += MovementSpeed;
		break;
	case GLFW_KEY_A:
		g_Camera.eye.x -= MovementSpeed;
		break;
	case GLFW_KEY_D:
		g_Camera.eye.x += MovementSpeed;
		break;
	case GLFW_KEY_Q:
		g_Camera.eye.y -= MovementSpeed;
		break;
	case GLFW_KEY_E:
		g_Camera.eye.y += MovementSpeed;
		break;
	/*
	RESET ALL GAME OBJECTS

	ENTER
	*/
	case GLFW_KEY_ENTER:
		for (std::vector<cGameObject*>::iterator itGO = ::g_vec_pGOs.begin(); itGO != ::g_vec_pGOs.end(); ++itGO) {
			CreateObject((*itGO), ::g_map_fileGOs[(*itGO)->getID()]);

			(*itGO)->Ax = 0.f;
			(*itGO)->Ay = 0.f;
			(*itGO)->Az = 0.f;

			(*itGO)->Vx = 0.f;
			(*itGO)->Vy = 0.f;
			(*itGO)->Vz = 0.f;
		}
		break;
	case GLFW_KEY_1:
		::g_Camera.eye = glm::vec3(0.f, 200.f, 250.f);
		break;
	case GLFW_KEY_2:
		::g_Camera.eye = glm::vec3(0.f, 400.f, 1.f);
		break;
	case GLFW_KEY_3:
		::g_Camera.eye = glm::vec3(-110.f, 200.f, 110.f);
		break;
	case GLFW_KEY_4:
		::g_Camera.eye = glm::vec3(-110.f, 60.f, 110.f);
		break;
	}

	std::stringstream ssTitle;
	ssTitle << "Camera: " << ::g_Camera.eye.x << ", " << ::g_Camera.eye.y << ", " << ::g_Camera.eye.z;
	glfwSetWindowTitle(window, ssTitle.str().c_str());

	return;
}

int main(void) {
	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);

	// Initialize the GLFW window
	InitWindow();

	// Display information about system
	std::cout << glGetString(GL_VENDOR) << " " << glGetString(GL_RENDERER) << ", " << glGetString(GL_VERSION) << std::endl;
	std::cout << "Shader language version: " << glGetString(GL_SHADING_LANGUAGE_VERSION) << std::endl;

	// Create shader objects
	::g_pTheShaderManager = new cShaderManager();
	cShaderManager::cShader verShader;
	cShaderManager::cShader fragShader;

	// Import the shaders from files
	verShader.fileName = "simpleVert.glsl";
	fragShader.fileName = "simpleFrag.glsl";
	if (!::g_pTheShaderManager->createProgramFromFile("simple", verShader, fragShader)) {
		std::cout << ::g_pTheShaderManager->getLastError();
		return -1;
	}

	// Setup the mesh object to mediate importing mesh from files
	::g_pTheMeshTypeManager = new cMeshTypeManager();
	shadProgID = ::g_pTheShaderManager->getIDFromFriendlyName("simple");

	// Read the scene description from a file
	ImportScene();

	// Get uniform locations from the shader
	mvp_location = glGetUniformLocation(shadProgID, "MVP");
	UniformLoc_ID_objectColour = glGetUniformLocation(shadProgID, "objectColour");
	UniformLoc_ID_isWireframe = glGetUniformLocation(shadProgID, "isWireframe");

	// Main application loop
	while (!glfwWindowShouldClose(window)) {
		// Update the apect ratio of the camera based on window size
		int width, height;
		glfwGetFramebufferSize(window, &width, &height);
		::g_Camera.aspect = width / (float)height;

		// Reset the viewport and clear the screen
		glViewport(0, 0, width, height);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		// Calculate the projection matrix and the view matrix
		p = glm::perspective(::g_Camera.fovy, ::g_Camera.aspect, ::g_Camera.zNear, ::g_Camera.zFar);
		v = glm::lookAt(::g_Camera.eye, ::g_Camera.center, ::g_Camera.up);

		// Physics and Collision processing
		PhysicsStep(0.01f);
		CollisionStep(0.01f);

		// Render loop
		DrawScene();

		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	// Cleanup objects
	delete ::g_pTheMeshTypeManager;
	delete ::g_pTheShaderManager;

	for (int i = 0; i < ::g_vec_pGOs.size(); ++i) {
		delete ::g_vec_pGOs[i];
	}

	// Final exit commands
	glfwDestroyWindow(window);
	glfwTerminate();
	exit(EXIT_SUCCESS);
}

glm::vec3 ClosestPtPointTriangle(glm::vec3 p, glm::vec3 a, glm::vec3 b, glm::vec3 c) {
	// Check if P in vertex region outside A
	glm::vec3 ab = b - a;
	glm::vec3 ac = c - a;
	glm::vec3 ap = p - a;
	float d1 = glm::dot(ab, ap);		// glm::dot( ab, ap );
	float d2 = glm::dot(ac, ap);
	if (d1 <= 0.0f && d2 <= 0.0f) return a; // barycentric coordinates (1,0,0)

											// Check if P in vertex region outside B
	glm::vec3 bp = p - b;
	float d3 = glm::dot(ab, bp);
	float d4 = glm::dot(ac, bp);
	if (d3 >= 0.0f && d4 <= d3) return b; // barycentric coordinates (0,1,0)

										  // Check if P in edge region of AB, if so return projection of P onto AB
	float vc = d1*d4 - d3*d2;
	if (vc <= 0.0f && d1 >= 0.0f && d3 <= 0.0f) {
		float v = d1 / (d1 - d3);
		return a + v * ab; // barycentric coordinates (1-v,v,0)
	}

	// Check if P in vertex region outside C
	glm::vec3 cp = p - c;
	float d5 = glm::dot(ab, cp);
	float d6 = glm::dot(ac, cp);
	if (d6 >= 0.0f && d5 <= d6) return c; // barycentric coordinates (0,0,1)

										  // Check if P in edge region of AC, if so return projection of P onto AC
	float vb = d5*d2 - d1*d6;
	if (vb <= 0.0f && d2 >= 0.0f && d6 <= 0.0f) {
		float w = d2 / (d2 - d6);
		return a + w * ac; // barycentric coordinates (1-w,0,w)
	}

	// Check if P in edge region of BC, if so return projection of P onto BC
	float va = d3*d6 - d5*d4;
	if (va <= 0.0f && (d4 - d3) >= 0.0f && (d5 - d6) >= 0.0f) {
		float w = (d4 - d3) / ((d4 - d3) + (d5 - d6));
		return b + w * (c - b); // barycentric coordinates (0,1-w,w)
	}

	// P inside face region. Compute Q through its barycentric coordinates (u,v,w)
	float denom = 1.0f / (va + vb + vc);
	float v = vb * denom;
	float w = vc * denom;
	return a + ab * v + ac * w; // = u*a + v*b + w*c, u = va * denom = 1.0f - v - w
}

void CollisionStep(float deltaTime) {
	::g_vec_Contacts.clear();

	// Test for Sphere to Sphere collisions
	for (std::vector<cGameObject*>::iterator itOut = ::g_vec_pGOs.begin(); itOut != ::g_vec_pGOs.end(); ++itOut) {
		if ((*itOut)->bIsUpdatedByCollision) {
			for (std::vector<cGameObject*>::iterator itIn = ::g_vec_pGOs.begin(); itIn != ::g_vec_pGOs.end(); ++itIn) {
				if ((*itIn)->bIsUpdatedByCollision) {
					if (itOut != itIn) {
						(*itOut)->solid_R = (*itOut)->solid_G = (*itOut)->solid_B = 1.f;
						(*itIn)->solid_R = (*itIn)->solid_G = (*itIn)->solid_B = 1.f;

						if (TestSphereSphereCollision((*itOut), (*itIn))) {
							cContact curContact;
							curContact.objectID = (*itOut)->getID();
							curContact.collisionType = cContact::WITH_SPHERE;
							::g_vec_Contacts.push_back(curContact);
							(*itOut)->objectCollisionFadeCounter = 1.f;
						}
					}
				}
			}
		}
	}

	// Reset game object colours
	for (std::vector<cGameObject*>::iterator itGO = ::g_vec_pGOs.begin(); itGO != ::g_vec_pGOs.end(); ++itGO) {
		if ((*itGO)->bIsUpdatedByCollision) {
			if ((*itGO)->groundCollisionFadeCounter <= 0.f && (*itGO)->objectCollisionFadeCounter <= 0.f) {
				(*itGO)->solid_R = (*itGO)->solid_G = (*itGO)->solid_B = 1.f;
			}
			else {
				(*itGO)->solid_R = (*itGO)->objectCollisionFadeCounter;
				(*itGO)->solid_G = (*itGO)->groundCollisionFadeCounter;
				(*itGO)->solid_B = 0.f;
				(*itGO)->objectCollisionFadeCounter -= 0.05f;
				(*itGO)->groundCollisionFadeCounter -= 0.05f;
			}
		}
	}

	// Test for Sphere to Triangle collisions
	for (std::vector<cGameObject*>::iterator itGO = ::g_vec_pGOs.begin(); itGO != ::g_vec_pGOs.end(); ++itGO) {
		if ((*itGO)->bIsUpdatedByCollision) {
			for (std::vector<cGameObject*>::iterator itEnvGO = ::g_vec_pGOs.begin(); itEnvGO != ::g_vec_pGOs.end(); ++itEnvGO) {
				if (!(*itEnvGO)->bIsUpdatedByPhysics) {
					std::vector<cPlyTriangle> meshTriInfo;
					::g_pTheMeshTypeManager->GetTriangleVertexInformation((*itEnvGO)->meshName, meshTriInfo);

					// Update the triangle information based on the actual location of the object
					for (int triIndex = 0; triIndex != meshTriInfo.size(); triIndex++) {
						// Current location of the object
						glm::vec3 itEnvGOLoc((*itEnvGO)->x, (*itEnvGO)->y, (*itEnvGO)->z);

						// Rotate the triangles on the X axis
						glm::vec4 v1RotX = glm::rotateX(glm::vec4(meshTriInfo[triIndex].v1, 1.f), (*itEnvGO)->post_Rot_X);
						glm::vec4 v2RotX = glm::rotateX(glm::vec4(meshTriInfo[triIndex].v2, 1.f), (*itEnvGO)->post_Rot_X);
						glm::vec4 v3RotX = glm::rotateX(glm::vec4(meshTriInfo[triIndex].v3, 1.f), (*itEnvGO)->post_Rot_X);

						// Update the triangle information
						meshTriInfo[triIndex].v1 = glm::vec3(v1RotX.x, v1RotX.y, v1RotX.z);
						meshTriInfo[triIndex].v2 = glm::vec3(v2RotX.x, v2RotX.y, v2RotX.z);
						meshTriInfo[triIndex].v3 = glm::vec3(v3RotX.x, v3RotX.y, v3RotX.z);

						// Rotate the triangles on the Y axis
						glm::vec4 v1RotY = glm::rotateY(glm::vec4(meshTriInfo[triIndex].v1, 1.f), (*itEnvGO)->post_Rot_Y);
						glm::vec4 v2RotY = glm::rotateY(glm::vec4(meshTriInfo[triIndex].v2, 1.f), (*itEnvGO)->post_Rot_Y);
						glm::vec4 v3RotY = glm::rotateY(glm::vec4(meshTriInfo[triIndex].v3, 1.f), (*itEnvGO)->post_Rot_Y);

						// Update the triangle information
						meshTriInfo[triIndex].v1 = glm::vec3(v1RotY.x, v1RotY.y, v1RotY.z);
						meshTriInfo[triIndex].v2 = glm::vec3(v2RotY.x, v2RotY.y, v2RotY.z);
						meshTriInfo[triIndex].v3 = glm::vec3(v3RotY.x, v3RotY.y, v3RotY.z);

						// Rotate the triangles on the Z axis
						glm::vec4 v1RotZ = glm::rotateZ(glm::vec4(meshTriInfo[triIndex].v1, 1.f), (*itEnvGO)->post_Rot_Z);
						glm::vec4 v2RotZ = glm::rotateZ(glm::vec4(meshTriInfo[triIndex].v2, 1.f), (*itEnvGO)->post_Rot_Z);
						glm::vec4 v3RotZ = glm::rotateZ(glm::vec4(meshTriInfo[triIndex].v3, 1.f), (*itEnvGO)->post_Rot_Z);

						// Update the triangle information
						meshTriInfo[triIndex].v1 = glm::vec3(v1RotZ.x, v1RotZ.y, v1RotZ.z);
						meshTriInfo[triIndex].v2 = glm::vec3(v2RotZ.x, v2RotZ.y, v2RotZ.z);
						meshTriInfo[triIndex].v3 = glm::vec3(v3RotZ.x, v3RotZ.y, v3RotZ.z);

						// Translate the triangles to the correct location based on the current object
						meshTriInfo[triIndex].v1 = meshTriInfo[triIndex].v1 + itEnvGOLoc;
						meshTriInfo[triIndex].v2 = meshTriInfo[triIndex].v2 + itEnvGOLoc;
						meshTriInfo[triIndex].v3 = meshTriInfo[triIndex].v3 + itEnvGOLoc;
					}

					// Test for the collision
					for (int triIndex = 0; triIndex != meshTriInfo.size(); triIndex++) {
						glm::vec3 ballCentre((*itGO)->x, (*itGO)->y, (*itGO)->z);
						glm::vec3 closestPoint = ClosestPtPointTriangle(ballCentre, meshTriInfo[triIndex].v1, meshTriInfo[triIndex].v2, meshTriInfo[triIndex].v3);

						if (glm::distance(ballCentre, closestPoint) < (*itGO)->radius) {
							cContact curContact;
							curContact.objectID = (*itGO)->getID();
							curContact.collisionType = cContact::WITH_TRIANGLE;
							(*itGO)->Vz = (*itEnvGO)->post_Rot_X * 25.f;
							(*itGO)->Vx = (*itEnvGO)->post_Rot_Z * -25.f;
							::g_vec_Contacts.push_back(curContact);

							(*itGO)->groundCollisionFadeCounter = 1.f;
						}
					}
				}
			}
		}
	}

	// Process the current collisions
	if (!::g_vec_Contacts.empty()) {
		for (std::vector<cContact>::iterator it = ::g_vec_Contacts.begin(); it != ::g_vec_Contacts.end(); ++it) {
			unsigned int ID = (*it).objectID;
			cGameObject* pCurGO = FindObjectByID(ID);

			if (pCurGO->groundCollisionFadeCounter <= 0.f) {
				pCurGO->solid_G = 0.f;
			}
			else {
				pCurGO->solid_G = pCurGO->groundCollisionFadeCounter;
				pCurGO->groundCollisionFadeCounter -= 0.01f;
			}

			if (pCurGO->objectCollisionFadeCounter <= 0.f) {
				pCurGO->solid_R = 0.f;
			}
			else {
				pCurGO->solid_R = pCurGO->objectCollisionFadeCounter;
				pCurGO->objectCollisionFadeCounter -= 0.01f;
			}
		}
	}

	// Update the objects if they have hit the surface
	for (std::vector<cGameObject*>::iterator itGO = ::g_vec_pGOs.begin(); itGO != ::g_vec_pGOs.end(); ++itGO) {
		if ((*itGO)->bIsUpdatedByCollision && (*itGO)->bIsUpdatedByPhysics) {
			if ((*itGO)->groundCollisionFadeCounter > 0.0f) {
				(*itGO)->Vy = -(*itGO)->Vy;
				(*itGO)->y = (*itGO)->y + (*itGO)->radius;
				(*itGO)->groundCollisionFadeCounter = 0.f;
			}
		}
	}

	return;
}

// Create the object from a string description using a string stream
void CreateObject(cGameObject* object, std::string description) {
	std::stringstream sstream(description);
	sstream >>
		object->meshName >>
		object->bIsControlledByUser >>
		object->scale >>
		object->radius >>
		object->x >>
		object->y >>
		object->z >>
		object->solid_R >>
		object->solid_G >>
		object->solid_B >>
		object->post_Rot_X >>
		object->post_Rot_Y >>
		object->post_Rot_Z >>
		object->bIsUpdatedByCollision >>
		object->bIsUpdatedByPhysics >>
		object->bIsWireframe;
}

// Update the scene for the next frame
void DrawScene() {
	// Loop through the object array
	for (std::vector<cGameObject*>::iterator itGO = ::g_vec_pGOs.begin(); itGO != ::g_vec_pGOs.end(); ++itGO) {
		std::string meshModelName = (*itGO)->meshName;

		// Get the mesh info for the current object
		GLuint VAO_ID = 0;
		int numberOfIndices = 0;
		float unitScale = 1.0f;
		if (!::g_pTheMeshTypeManager->LookUpMeshInfo(meshModelName, VAO_ID, numberOfIndices, unitScale)) {
			continue;
		}

		// Setup the model * view * projection matrix and model matrix
		glm::mat4x4 mvp(1.0f);
		glm::mat4x4 m = glm::mat4x4(1.0f);

		// Pre-rotation
		m = glm::rotate(m, (*itGO)->pre_Rot_X, glm::vec3(1.0f, 0.0f, 0.0f));
		m = glm::rotate(m, (*itGO)->pre_Rot_Y, glm::vec3(0.0f, 1.0f, 0.0f));
		m = glm::rotate(m, (*itGO)->pre_Rot_Z, glm::vec3(0.0f, 0.0f, 1.0f));

		// Translation
		m = glm::translate(m, glm::vec3((*itGO)->x, (*itGO)->y, (*itGO)->z));

		// Post-rotation
		m = glm::rotate(m, (*itGO)->post_Rot_X, glm::vec3(1.0f, 0.0f, 0.0f));
		m = glm::rotate(m, (*itGO)->post_Rot_Y, glm::vec3(0.0f, 1.0f, 0.0f));
		m = glm::rotate(m, (*itGO)->post_Rot_Z, glm::vec3(0.0f, 0.0f, 1.0f));

		float actualScale = (*itGO)->scale * unitScale;

		// Scale
		m = glm::scale(m, glm::vec3(actualScale, actualScale, actualScale));

		// Determine if object is to be drawn as a wireframe
		if ((*itGO)->bIsWireframe) {
			glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
			glDisable(GL_CULL_FACE);
			glEnable(GL_DEPTH_TEST);
		}
		else {
			glCullFace(GL_BACK);
			glEnable(GL_CULL_FACE);
			glEnable(GL_DEPTH_TEST);
			glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
		}

		// Calculate model * view * projection matrix
		mvp = p * v * m;

		// Update shader information
		::g_pTheShaderManager->useShaderProgram("simple");

		glUniformMatrix4fv(mvp_location, 1, GL_FALSE, (const GLfloat*)glm::value_ptr(mvp));
		glUniform3f(UniformLoc_ID_objectColour, (*itGO)->solid_R, (*itGO)->solid_G, (*itGO)->solid_B);

		if ((*itGO)->bIsWireframe) {
			glUniform1i(UniformLoc_ID_isWireframe, TRUE);
		}
		else {
			glUniform1i(UniformLoc_ID_isWireframe, FALSE);
		}

		glBindVertexArray(VAO_ID);
		glDrawElements(GL_TRIANGLES, numberOfIndices, GL_UNSIGNED_INT, (GLvoid*)0);
		glBindVertexArray(0);
	}

	return;
}

// Lookup an object based on an ID
cGameObject* FindObjectByID(unsigned int ID) {
	for (std::vector<cGameObject*>::iterator itGO = ::g_vec_pGOs.begin(); itGO != ::g_vec_pGOs.end(); ++itGO) {
		if ((*itGO)->getID() == ID) {
			return (*itGO);
		}
	}

	return 0;
}

// Read the scene description from a file
void ImportScene() {
	// Array of strings to read in the object attributes
	std::string line;
	std::getline(std::cin, line); // Header

	while (std::getline(std::cin, line)) {
		cGameObject* pObject = new cGameObject();

		CreateObject(pObject, line);

		::g_map_fileGOs[pObject->getID()] = line;

		// Get the mesh name and determine if it still needs to be imported
		GLuint VAO_ID = 0;
		int numberOfIndices = 0;
		float unitScale = 1.0f;
		if (!::g_pTheMeshTypeManager->LookUpMeshInfo(pObject->meshName, VAO_ID, numberOfIndices, unitScale)) {
			::g_pTheMeshTypeManager->LoadPlyFileIntoGLBuffer(shadProgID, pObject->meshName, !pObject->bIsUpdatedByPhysics);
		}

		// Add the resulting object into the vector of all objects
		::g_vec_pGOs.push_back(pObject);
	}
	// Cleanup the attribute array

	return;
}

// GLFW window creation
void InitWindow() {
	glfwSetErrorCallback(error_callback);

	if (!glfwInit()) {
		exit(EXIT_FAILURE);
	}

	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 2);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);
	window = glfwCreateWindow(1200, 800, "Simple example", NULL, NULL);
	if (!window) {
		glfwTerminate();
		exit(EXIT_FAILURE);
	}

	glfwSetKeyCallback(window, key_callback);
	glfwMakeContextCurrent(window);
	gladLoadGLLoader((GLADloadproc)glfwGetProcAddress);
	glfwSwapInterval(1);

	return;
}

// Physics update
void PhysicsStep(float deltaTime) {
	glm::vec3 gravityForce(0.0f, -90.81, 0.0f);

	for (std::vector<cGameObject*>::iterator itGO = ::g_vec_pGOs.begin(); itGO != ::g_vec_pGOs.end(); ++itGO) {
		if ((*itGO)->bIsUpdatedByPhysics) {
			// If the object has gone off the platform reset the object from the description in the file
			if ((*itGO)->x > 250.f || (*itGO)->x < -250.f || (*itGO)->y > 250.f || (*itGO)->y < -250.f || (*itGO)->z > 250.f || (*itGO)->z < -250.f) {
				CreateObject((*itGO), ::g_map_fileGOs[(*itGO)->getID()]);

				(*itGO)->Ax = 0.f;
				(*itGO)->Ay = 0.f;
				(*itGO)->Az = 0.f;

				(*itGO)->Vx = 0.f;
				(*itGO)->Vy = 0.f;
				(*itGO)->Vz = 0.f;
			}

			// Update the object with the gravity force and delta time
			(*itGO)->Vx += ((*itGO)->Ax + gravityForce.x) * deltaTime;
			(*itGO)->Vy += ((*itGO)->Ay + gravityForce.y) * deltaTime;
			(*itGO)->Vz += ((*itGO)->Az + gravityForce.z) * deltaTime;

			(*itGO)->lastX = (*itGO)->x;
			(*itGO)->lastY = (*itGO)->y;
			(*itGO)->lastZ = (*itGO)->z;

			(*itGO)->x += (*itGO)->Vx * deltaTime;
			(*itGO)->y += (*itGO)->Vy * deltaTime;
			(*itGO)->z += (*itGO)->Vz * deltaTime;
		}
	}

	return;
}

// Test for a sphere penetrating another sphere
bool TestSphereSphereCollision(cGameObject * pBallA, cGameObject * pBallB) {
	float totalRadius = pBallA->radius + pBallB->radius;
	float distance = glm::distance(
		glm::vec3(pBallA->x, pBallA->y, pBallA->z),
		glm::vec3(pBallB->x, pBallB->y, pBallB->z)
	);

	if (distance <= totalRadius) {
		return true;
	}

	return false;
}
