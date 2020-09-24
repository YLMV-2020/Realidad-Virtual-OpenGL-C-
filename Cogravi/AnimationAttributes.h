#ifndef ANIMATION_ATTRIBUTES_HEADER
#define ANIMATION_ATTRIBUTES_HEADER

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include "DynamicGameObject.h"
#include "MeshA.h"

namespace Cogravi
{
	class AnimationAttributes
	{
	public:

		//Model

		int userIndex;

		btRigidBody* body;
		btCollisionShape* shape;
		
		glm::vec3 translate;		
		glm::mat4 physics_matrix;

		//Collider

		int shape_current;
		float margin = 1.0f;
		glm::vec3 shapeScalar = glm::vec3(1.0f);

		//RigidBody

		glm::vec3 angularFactor = glm::vec3(0);
		glm::vec3 linearVelocity = glm::vec3(0);

		glm::vec3 localInertia = glm::vec3(0);
		glm::vec3 linearFactor = glm::vec3(1);

		glm::vec3 gravity = glm::vec3(0, 1, 0);

		float mass = 1.0f;
		float restitution = 1.0f;

	};
}
#endif