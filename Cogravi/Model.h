#ifndef MODEL_HEADER
#define MODEL_HEADER

#include "ModelAttributes.h"

namespace Cogravi
{
	class Model :public ModelAttributes, public GameObject
	{
	public:		
	
		Model(glm::vec3 position, glm::vec3 rotation, glm::vec3 scale, string const& path, Shader shader, vector<Texture> textures = {}) :
			GameObject(position, rotation, scale, path, shader, textures)
		{
			
			this->translate = glm::vec3(0.0f, 0.0f, 0.0f);
		}

		~Model(){}

		void render(Camera& camera) override
		{
			shader.use();

			glm::mat4 view = camera.GetViewMatrix();
			glm::mat4 projection = glm::perspective(glm::radians(camera.FOV), (float)4.0f / (float)3.0f, camera.NEAR, camera.FAR);

			transform = glm::mat4(1.0f);
			
			//transform = transform * physics_matrix;
			//transform = glm::translate(transform, glm::vec3(position.x , position.y , position.z ));
			transform = glm::translate(transform, glm::vec3(position.x + translate.x, position.y + translate.y, position.z + translate.z));

			transform = glm::rotate(transform, glm::radians(rotation.y), glm::vec3(0.0f, 1.0f, 0.0f));
			transform = glm::rotate(transform, glm::radians(rotation.z), glm::vec3(0.0f, 0.0f, 1.0f));
			transform = glm::rotate(transform, glm::radians(rotation.x), glm::vec3(1.0f, 0.0f, 0.0f));

			transform = glm::scale(transform, scale);

			shader.setVec3("lightPos", glm::vec3(2, 4, 0));
			shader.setVec3("viewPos", camera.Position);
			
			shader.setMat4("model", transform);
			shader.setMat4("view", view);
			shader.setMat4("projection", projection);

			draw(shader);
		}

		void render(Avatar& avatar) 
		{
			shader.use();

			glm::mat4 view = avatar.view;
			glm::mat4 projection = avatar.proj;

			transform = glm::mat4(1.0f);

			//transform = transform * physics_matrix;
			//transform = glm::translate(transform, glm::vec3(position.x , position.y , position.z ));
			transform = glm::translate(transform, glm::vec3(position.x + translate.x, position.y + translate.y, position.z + translate.z));

			transform = glm::rotate(transform, glm::radians(rotation.y), glm::vec3(0.0f, 1.0f, 0.0f));
			transform = glm::rotate(transform, glm::radians(rotation.z), glm::vec3(0.0f, 0.0f, 1.0f));
			transform = glm::rotate(transform, glm::radians(rotation.x), glm::vec3(1.0f, 0.0f, 0.0f));

			transform = glm::scale(transform, scale);

			shader.setVec3("lightPos", glm::vec3(2, 4, 0));
			shader.setVec3("viewPos", glm::vec3(avatar.position.x, avatar.position.y, avatar.position.z));

			shader.setMat4("model", transform);
			shader.setMat4("view", view);
			shader.setMat4("projection", projection);

			draw(shader);
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
			btCollisionShape* nshape = new btCapsuleShape(btScalar(1.0f), btScalar(0.2f));
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

		void changeBodyPhysics(BulletWorldController* worldController)
		{
			worldController->dynamicsWorld->removeRigidBody(body);
			body = NULL;
			shape = NULL;

			switch (shape_current)
			{
			case 0:
				addBodyPhysicsBox(userIndex, worldController);
				break;
			case 1:
				addBodyPhysicsSphere(userIndex, worldController);
				break;
			case 2:
				addBodyPhysicsCapsule(userIndex, worldController);
				break;
			case 3:
				addBodyPhysicsCylinder(userIndex, worldController);
				break;
			case 4:
				addBodyPhysicsCone(userIndex, worldController);
				break;
			default:
				break;
			}			
				
		}

		void destroy(BulletWorldController* worldController)
		{
			//Obtenemos todos los componentes de colision del mundo
			btCollisionObjectArray arr = worldController->dynamicsWorld->getCollisionObjectArray();

			//ID del cuerpo que deseamos destruir
			int deleteID;

			//Buscamos entre todos los componentes aquel con el ID que se desea borrar, guardamos su ID y lo eliminamos del mundo
			for (size_t i = 0; i < arr.size(); i++)
			{
				if (arr.at(i)->getUserIndex() == userIndex) deleteID = i;
			}
			worldController->dynamicsWorld->removeCollisionObject(worldController->dynamicsWorld->getCollisionObjectArray()[deleteID]);
		}

		void changeAngularFactor() 
		{ 
			body->setAngularFactor(btVector3(angularFactor.x, angularFactor.y, angularFactor.z));
		}

		void changeLinearVelocity() 
		{ 
			body->setLinearVelocity(btVector3(linearVelocity.x, linearVelocity.y, linearVelocity.z));
		}

		void changeMass()
		{
			body->setMassProps(mass, btVector3(localInertia.x, localInertia.y, localInertia.z));
		}

		void changeMargin()
		{
			shape->setMargin(margin);
		}

		void changeScalar()
		{
			shape->setLocalScaling(btVector3(shapeScalar.x, shapeScalar.y, shapeScalar.z));
		}

		void change()
		{
			btTransform t;
			t.setIdentity();
			
			t.setFromOpenGLMatrix(glm::value_ptr(transform));

			body->setCenterOfMassTransform(t);
		}


	};
}
#endif