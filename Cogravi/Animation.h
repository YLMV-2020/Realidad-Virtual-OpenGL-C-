#ifndef ANIMATION_HEADER
#define ANIMATION_HEADER

#include "AnimationAttributes.h"

namespace Cogravi
{
	class Animation :public AnimationAttributes, public DynamicGameObject
	{
	public:

		Animation(glm::vec3 position, glm::vec3 rotation, glm::vec3 scale, string const& path, Shader shader, vector<Texture> textures = {}) :
			DynamicGameObject(position, rotation, scale, path, shader, textures)
		{
			this->translate = glm::vec3(0.0f, 0.0f, 0.0f);		
		}

		void render(Camera& camera, Shader &shader, float animationTime) override
		{
			DynamicGameObject::render(camera, shader, animationTime);
		}

		void render(Avatar& avatar, Shader& shader, float animationTime) override
		{
			DynamicGameObject::render(avatar, shader, animationTime);
		}

		void update() override
		{
			btTransform physics_transform;
			body->getMotionState()->getWorldTransform(physics_transform);
			glm::mat4 graphics_transform;
			physics_transform.getOpenGLMatrix(glm::value_ptr(physics_matrix));
			position = glm::vec3(physics_matrix[3][0], 0, physics_matrix[3][2]);

		}

		void addBodyPhysicsBox(int userIndex, BulletWorldController* worldController)
		{
			this->userIndex = userIndex;
			btCollisionShape* nshape = new btBoxShape(btVector3(1.0f, 1.0f, 1.0f));
			this->shape = nshape;
			bodyPhysicsConfiguration(worldController);
		}

		void addBodyPhysicsSphere(int userIndex, BulletWorldController* worldController)
		{
			this->userIndex = userIndex;
			btCollisionShape* nshape = new btSphereShape(btScalar(1));
			this->shape = nshape;
			bodyPhysicsConfiguration(worldController);
		}

		void addBodyPhysicsCapsule(int userIndex, BulletWorldController* worldController)
		{
			this->userIndex = userIndex;
			btCollisionShape* nshape = new btCapsuleShape(btScalar(1.0f), btScalar(1.0f));
			this->shape = nshape;
			bodyPhysicsConfiguration(worldController);
		}

		void addBodyPhysicsCylinder(int userIndex, BulletWorldController* worldController)
		{
			this->userIndex = userIndex;
			btCollisionShape* nshape = new btCylinderShape(btVector3(1.0f, 1.0f, 1.0f));
			this->shape = nshape;
			bodyPhysicsConfiguration(worldController);
		}

		void addBodyPhysicsCone(int userIndex, BulletWorldController* worldController)
		{
			this->userIndex = userIndex;
			btCollisionShape* nshape = new btConeShape(btScalar(1.0f), btScalar(1.0f));
			this->shape = nshape;
			bodyPhysicsConfiguration(worldController);
		}

		void bodyPhysicsConfiguration(BulletWorldController* worldController)
		{
			btTransform transform;
			transform.setIdentity();
			transform.setOrigin(btVector3(position.x, position.y, position.z));
			transform.setRotation(btQuaternion(rotation.x, rotation.y, rotation.z, 1));

			//Calculamos la inercia del modelo
			btVector3 inertia(localInertia.x, localInertia.y, localInertia.z);
			shape->calculateLocalInertia(mass, inertia);

			//Configuramos las propiedades básicas de construcción del cuerpo

			btDefaultMotionState* state = new btDefaultMotionState(transform);
			btRigidBody::btRigidBodyConstructionInfo info(mass, state, shape, inertia);

			//Establecemos los parámetros que recibidos como parámetro
			body = new btRigidBody(info);
			body->setRestitution(restitution);
			body->setUserIndex(this->userIndex);
			body->setLinearFactor(btVector3(linearFactor.x, linearFactor.y, linearFactor.z));

			//Por defecto, todos los modelos están bloqueados en el espacio en X,Z así como sus ejes de rotación
			body->setAngularFactor(btVector3(angularFactor.x, angularFactor.y, angularFactor.z));
			body->setGravity(btVector3(gravity.x, gravity.y, gravity.z));

			body->setActivationState(DISABLE_DEACTIVATION);

			//Añadimos el cuerpo al mundo dinámico
			worldController->dynamicsWorld->addRigidBody(body);
			//worldController->dynamicsWorld->removeRigidBody();

			//Guardamos la información del modelo en vectores de punteros
			//worldController->rigidBodies.push_back(body);
			//worldController->motionStates.push_back(state);
			//worldController->collisionShapes.push_back(shape);

		}

		
		
	};
}



#endif