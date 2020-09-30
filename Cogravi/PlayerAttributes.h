#ifndef PLAYER_ATTRIBUTES_HEADER
#define PLAYER_ATTRIBUTES_HEADER

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include <BulletDynamics/Character/btKinematicCharacterController.h>
#include <BulletCollision/CollisionDispatch/btGhostObject.h>

namespace Cogravi
{
	class PlayerAttributes
	{
	public:

		glm::vec3 position;
		glm::vec3 rotation;

		float RUN_SPEED = 10.0f;
		float TURN_SPEED = 180.0f;
		float GRAVITY = -50.0f;
		float JUMP_POWER = 20.0f;

		float TERRAIN_HEIGHT = 0.0f;

		float currentSpeed = 0.0f;
		float currentTurnSpeed = 0.0f;
		float upwardsSpeed = 0.0f;

		bool isFloor = true;
		bool isJump = false;

		btRigidBody* body;
		btCapsuleShape* shape;

		btPairCachingGhostObject* ghostObject;
		//btBoxShape* convexShape = new btBoxShape(btVector3(1, 1, 1));

		//For now only a simple test that it initializes correctly.
		btKinematicCharacterController* tested;

		
	};
}
#endif