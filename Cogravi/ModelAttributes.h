#ifndef MODEL_ATTRIBUTES_HEADER
#define MODEL_ATTRIBUTES_HEADER

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include "GameObject.h"
#include "MeshM.h"

namespace Cogravi
{
	struct Description
	{
		char bloc[500];
		vector<string> printf;
		GLuint printfSize;
		glm::vec3 color = { 0.0f, 0.0f, 0.0f };
		glm::vec3 colorBG = { 0.764f, 0.764f, 0.764f };
		GLint font = 0;
		string fontPath = "Arial.ttf";
	};

	class ModelAttributes
	{
	public:

		//Model

		int userIndex;

		btRigidBody* body;
		btCollisionShape* shape;

		glm::vec3 translate;
		glm::vec3 rotate;
		glm::mat4 physicsMatrix;

		Description description;
		ColliderType colliderType;

		//Collider

		int shape_current;
		float margin = 1.0f;
		glm::vec3 shapeScalar = glm::vec3(0.10f);
		bool isCollision = false;

		//RigidBody

		bool angularFactor[3];
		glm::vec3 linearVelocity = glm::vec3(0);

		glm::vec3 localInertia = glm::vec3(0);
		glm::vec3 linearFactor = glm::vec3(1);

		glm::vec3 gravity = glm::vec3(0, -9.81f, 0);

		float mass = 1.0f;
		float restitution = 1.0f;
		
	};
}
#endif