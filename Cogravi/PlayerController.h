#ifndef PLAYER_CONTROLLER_HEADER
#define PLAYER_CONTROLLER_HEADER

#include "PlayerAttributes.h"

namespace Cogravi
{
	class PlayerController : public PlayerAttributes
	{
	public:			

		PlayerController(glm::vec3 position, glm::vec3 rotation, glm::vec3 scale, string const& path, vector<Texture> textures, Shader shader, BulletWorldController*& bulletWorldController) 
			//Animation(position, rotation, scale, path, textures, shader)
		{		

			addBodyPhysicsCapsule(bulletWorldController);

			/*ghostObject = new btPairCachingGhostObject();
			ghostObject->setCollisionShape(shape);
			ghostObject->setUserPointer(this);
			ghostObject->setCollisionFlags(btCollisionObject::CF_NO_CONTACT_RESPONSE);
			bulletWorldController->dynamicsWorld->addCollisionObject(ghostObject, btBroadphaseProxy::KinematicFilter, btBroadphaseProxy::StaticFilter | btBroadphaseProxy::DefaultFilter);
			tested = new btKinematicCharacterController(ghostObject, shape, 1);
			bulletWorldController->dynamicsWorld->addCharacter(tested);*/
		}

		void addBodyPhysicsCapsule(BulletWorldController* worldController)
		{
			btCapsuleShape* nshape = new btCapsuleShape(btScalar(1.0f), btScalar(1.0f));
			this->shape = nshape;
			bodyPhysicsConfiguration(worldController);
		}

		void bodyPhysicsConfiguration(BulletWorldController* worldController)
		{
			btTransform transform;
			transform.setIdentity();
			transform.setOrigin(btVector3(-30, 5.0f, 15.f));
			transform.setRotation(btQuaternion(0, 0, 0, 1));

			//Calculamos la inercia del modelo
			btVector3 inertia(0, 0, 0);
			shape->calculateLocalInertia(70, inertia);

			//Configuramos las propiedades básicas de construcción del cuerpo

			btDefaultMotionState* state = new btDefaultMotionState(transform);
			btRigidBody::btRigidBodyConstructionInfo info(70, state, shape, inertia);

			//Establecemos los parámetros que recibidos como parámetro
			body = new btRigidBody(info);
			body->setRestitution(1);
			body->setUserIndex(200);
			body->setLinearFactor(btVector3(1, 1, 1));

			body->setSleepingThresholds(0.0, 0.0);
			body->setAngularFactor(0.0);

			//Por defecto, todos los modelos están bloqueados en el espacio en X,Z así como sus ejes de rotación
			//body->setAngularFactor(btVector3(1, 1, 1));
			body->setGravity(btVector3(0, 1, 0));

			body->setActivationState(DISABLE_DEACTIVATION);

			//Añadimos el cuerpo al mundo dinámico
			worldController->dynamicsWorld->addRigidBody(body);

		}

		void update(GLFWwindow* window)
		{
			// Synch ghost with actually object
			//m_pGhostObject->getWorldTransform().getOrigin().setY(m_pGhostObject->getWorldTransform().getOrigin().getY() - 0.01f);

			// Update transform
			//m_pMotionState->getWorldTransform(m_motionTransform);

			btTransform T;

			T = body->getCenterOfMassTransform();

			//T.setRotation(btQuaternion(rotation.x, rotation.y, rotation.z, 1));
			T.setRotation(btQuaternion(glm::radians(rotation.x), glm::radians(rotation.y), glm::radians(rotation.z), 1));

			body->setCenterOfMassTransform(T);

			if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS)
			{
				//ghostObject->setWorldTransform(body->getWorldTransform());

				////cout << "Saltando\n";
				////body->applyCentralImpulse(btVector3(0, 70, 0));
				//tested->jump(btVector3(0, 70, 0));
				//if (ghostObject->isKinematicObject()) cout << "Yes\n";
				body->applyCentralImpulse(btVector3(0, 30, 0));
				if (isFloor)
				{
					//this->upwardsSpeed = JUMP_POWER;
					isFloor = false;
				}
			}
			if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS)
			{
				body->applyCentralImpulse(btVector3(position.x*200, 0, position.z*200));
				this->currentSpeed = RUN_SPEED;
			}
			else if(glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS)
			{
				this->currentSpeed = -RUN_SPEED;
			}
			else 
			{
				body->applyCentralImpulse(btVector3(0, 0, 0));
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

		}
		
		void move(float deltaTime)
		{
			rotation += glm::vec3(0, currentTurnSpeed * deltaTime, 0);
			float distance = currentSpeed * deltaTime;

			float dx = distance * glm::sin(glm::radians(rotation.y));
			float dz = distance * glm::cos(glm::radians(rotation.y));

			cout << "DX: " << dx << "\n";

			position = glm::vec3(dx, 0, dz);
			upwardsSpeed += GRAVITY * deltaTime;
			//position += glm::vec3(0, upwardsSpeed * deltaTime, 0);

			if (position.y <= TERRAIN_HEIGHT)
			{
				upwardsSpeed = 0.0f;
				position.y = TERRAIN_HEIGHT;
				isFloor = true;
			}
		}
	
		~PlayerController()
		{

		}
	};
}
#endif