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

		BulletWorldController* bulletWorldController;
		DebugDrawer* debugDrawer;

		float RUN_SPEED = 10.0f;
		float TURN_SPEED = 160.0f;
		float GRAVITY = -50.0f;
		float JUMP_POWER = 20.0f;

		float TERRAIN_HEIGHT = 0.0f;

		float currentSpeed = 0.0f;
		float currentTurnSpeed = 0.0f;
		float upwardsSpeed = 0.0f;

		bool isFloor = true;
		bool isJump = false;
		
		btKinematicCharacterController* character;
		btConvexShape* m_pCollisionShape;
		btDefaultMotionState* m_pMotionState;
		btRigidBody* m_pRigidBody;
		btPairCachingGhostObject* m_pGhostObject;

		bool m_onGround;
		//bool m_onJumpableGround; // A bit lower contact than just onGround
		bool m_hittingWall;

		float m_bottomYOffset;
		float m_bottomRoundedRegionYOffset;

		float m_stepHeight;

		btTransform m_motionTransform;

		glm::vec3 m_manualVelocity;
		std::vector<glm::vec3> m_surfaceHitNormals;

		btVector3 m_previousPosition;

		float m_jumpRechargeTimer;

		void ParseGhostContacts();

		void UpdatePosition();
		void UpdateVelocity();


		float m_deceleration;
		float m_maxSpeed;
		float m_jumpImpulse = 50;

		float m_jumpRechargeTime;

		void Walk(const glm::vec2 dir);

		// Ignores y
		void Walk(const glm::vec3 dir);

		void Update();

		void Jump();

		glm::vec3 GetPosition() const;
		glm::vec3 GetVelocity() const;

		bool IsOnGround() const;

	};
}
#endif