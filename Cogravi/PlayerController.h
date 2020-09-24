#ifndef PLAYER_CONTROLLER_HEADER
#define PLAYER_CONTROLLER_HEADER

#include "PlayerAttributes.h"

namespace Cogravi
{
	class PlayerController : public PlayerAttributes
	{
	public:			

		PlayerController(glm::vec3 position, glm::vec3 rotation, glm::vec3 scale, string const& path, vector<Texture> textures, Shader shader, BulletWorldController*& bulletWorldController, DebugDrawer*& debugDrawer) 
			//Animation(position, rotation, scale, path, textures, shader)
		{

			this->bulletWorldController = bulletWorldController;
			this->debugDrawer = debugDrawer;

			/*addBodyPhysicsCapsule(-1, bulletWorldController);
			shape->setLocalScaling(btVector3(0.35f, 0.6f, 0.0f));*/

			btTransform transform;
			transform.setIdentity();
			transform.setOrigin(btVector3(position.x, position.y, position.z));
			transform.setRotation(btQuaternion(rotation.x, rotation.y, rotation.z, 1));

			m_pCollisionShape = new btCapsuleShape(1, 1);

			m_pMotionState = new btDefaultMotionState(transform);

			btVector3 intertia;
			m_pCollisionShape->calculateLocalInertia(1, intertia);

			btRigidBody::btRigidBodyConstructionInfo rigidBodyCI(1, m_pMotionState, m_pCollisionShape, intertia);

			// No friction, this is done manually
			rigidBodyCI.m_friction = 0.0f;
			//rigidBodyCI.m_additionalDamping = true;
			//rigidBodyCI.m_additionalLinearDampingThresholdSqr= 1.0f;
			//rigidBodyCI.m_additionalLinearDampingThresholdSqr = 0.5f;
			rigidBodyCI.m_restitution = 0.0f;

			rigidBodyCI.m_linearDamping = 0.0f;

			m_pRigidBody = new btRigidBody(rigidBodyCI);

			// Keep upright
			m_pRigidBody->setAngularFactor(0.0f);

			m_pRigidBody->setActivationState(DISABLE_DEACTIVATION);

			bulletWorldController->dynamicsWorld->addRigidBody(m_pRigidBody);

			m_pGhostObject = new btPairCachingGhostObject();

			m_pGhostObject->setCollisionShape(m_pCollisionShape);
			m_pGhostObject->setCollisionFlags(btCollisionObject::CF_CHARACTER_OBJECT);

			character = new btKinematicCharacterController(m_pGhostObject, m_pCollisionShape, 1);
			character->debugDraw(debugDrawer);

			bulletWorldController->dynamicsWorld->addCollisionObject(m_pGhostObject, btBroadphaseProxy::KinematicFilter, btBroadphaseProxy::StaticFilter | btBroadphaseProxy::DefaultFilter);
			bulletWorldController->dynamicsWorld->addAction(character);
			bulletWorldController->dynamicsWorld->addCharacter(character);
			//bulletWorldController->dynamicsWorld->addVehicle
			
		}

		void update(GLFWwindow* window)
		{

			if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS)
			{
				if (isFloor)
				{
					character->updateAction(bulletWorldController->dynamicsWorld, 1);
					character->jump(btVector3(0, 20, 0));
					//character->jump(btVector3(0, 10, 0));
					//m_pRigidBody->applyCentralImpulse(btVector3(0.0f, m_jumpImpulse, 0.0f));
					this->upwardsSpeed = JUMP_POWER;
					isFloor = false;
				}
			}

			if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS)
			{
				this->currentSpeed = RUN_SPEED;
			}
			else if(glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS)
			{
				this->currentSpeed = -RUN_SPEED;
			}
			else 
			{
				this->currentSpeed = 0.0f;
			}

			if (glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS)
			{
				this->currentTurnSpeed = TURN_SPEED;
			}
			else if (glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS)
			{
				this->currentTurnSpeed = -TURN_SPEED;
			}		 
			else
			{
				this->currentTurnSpeed = 0.0f;
			}

			//m_pGhostObject->setWorldTransform(m_pRigidBody->getWorldTransform());
			
			
			/*float height = body->getWorldTransform().getOrigin().getY();
			float heightTarget = height > position.y ? height : position.y + 1;

			btTransform target;

			target.setIdentity();
			target.setOrigin(btVector3(position.x, position.y + 1, position.z));

			btQuaternion quat;
			quat.setEulerZYX(btScalar(glm::radians(rotation.z)), btScalar(glm::radians(rotation.y)), btScalar(glm::radians(rotation.x)));
			target.setRotation(btQuaternion(quat));

			body->setWorldTransform(target);*/

		}
		
		void move(float deltaTime)
		{
			/*rotation += glm::vec3(0, currentTurnSpeed * deltaTime, 0);
			float distance = currentSpeed * deltaTime;

			float dx = distance * glm::sin(glm::radians(rotation.y));
			float dz = distance * glm::cos(glm::radians(rotation.y));

			position += glm::vec3(dx, 0, dz);
			upwardsSpeed += GRAVITY * deltaTime;
			position += glm::vec3(0, upwardsSpeed * deltaTime, 0);

			if (position.y <= TERRAIN_HEIGHT)
			{
				upwardsSpeed = 0.0f;
				position.y = TERRAIN_HEIGHT;
				isFloor = true;
			}*/
		}
	
		~PlayerController()
		{

		}
	};
}
#endif