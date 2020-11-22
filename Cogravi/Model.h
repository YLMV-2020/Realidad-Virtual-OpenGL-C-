#ifndef MODEL_HEADER
#define MODEL_HEADER

#include "ModelAttributes.h"

namespace Cogravi
{
	class Model :public ModelAttributes, public GameObject
	{
	public:		
		
		Model(glm::vec3 position, glm::vec3 rotation, glm::vec3 scale, string const& path, vector<Texture> textures = {}) :
			GameObject(position, rotation, scale, path, textures)
		{
			this->translate = glm::vec3(0.0f, 0.0f, 0.0f);
		}

		~Model(){}

		void render(Camera& camera, Shader& shader) override
		{
			shader.use();

			glm::mat4 view = camera.GetViewMatrix();
			glm::mat4 projection = camera.GetProjectionMatrix();

			transform = glm::mat4(1.0f);
			
			transform = transform * physicsMatrix;
			//transform = glm::translate(transform, glm::vec3(position.x , position.y , position.z ));
			transform = glm::translate(transform, glm::vec3(translate.x, translate.y, translate.z));
			//transform = glm::translate(transform, glm::vec3(position.x + translate.x, position.y + translate.y, position.z + translate.z));

			transform = glm::rotate(transform, glm::radians(rotation.y), glm::vec3(0.0f, 1.0f, 0.0f));
			transform = glm::rotate(transform, glm::radians(rotation.z), glm::vec3(0.0f, 0.0f, 1.0f));
			transform = glm::rotate(transform, glm::radians(rotation.x), glm::vec3(1.0f, 0.0f, 0.0f));

			transform = glm::scale(transform, scale);

			shader.setVec3("viewPos", camera.Position);
			
			shader.setMat4("model", transform);
			shader.setMat4("view", view);
			shader.setMat4("projection", projection);

			draw(shader);
		}

		void render(Avatar& avatar, Shader& shader)
		{
			shader.use();

			glm::mat4 view = avatar.GetViewMatrix();
			glm::mat4 projection = avatar.GetProjectionMatrix();

			transform = glm::mat4(1.0f);

			transform = transform * physicsMatrix;
			transform = glm::translate(transform, glm::vec3(translate.x, translate.y, translate.z));

			transform = glm::rotate(transform, glm::radians(rotation.y), glm::vec3(0.0f, 1.0f, 0.0f));
			transform = glm::rotate(transform, glm::radians(rotation.z), glm::vec3(0.0f, 0.0f, 1.0f));
			transform = glm::rotate(transform, glm::radians(rotation.x), glm::vec3(1.0f, 0.0f, 0.0f));

			transform = glm::scale(transform, scale);

			shader.setVec3("viewPos", glm::vec3(avatar.position.x, avatar.position.y, avatar.position.z));

			shader.setMat4("model", transform);
			shader.setMat4("view", view);
			shader.setMat4("projection", projection);

			draw(shader);
		}

		void renderFramebuffer(Camera& camera, Shader& shader, unsigned int meshIndex)
		{
			shader.use();

			glm::mat4 view = camera.GetViewMatrix();
			glm::mat4 projection = camera.GetProjectionMatrix();

			transform = glm::mat4(1.0f);

			transform = transform * physicsMatrix;
			transform = glm::translate(transform, glm::vec3(translate.x, translate.y, translate.z));
		
			transform = glm::rotate(transform, glm::radians(rotation.y), glm::vec3(0.0f, 1.0f, 0.0f));
			transform = glm::rotate(transform, glm::radians(rotation.z), glm::vec3(0.0f, 0.0f, 1.0f));
			transform = glm::rotate(transform, glm::radians(rotation.x), glm::vec3(1.0f, 0.0f, 0.0f));

			transform = glm::scale(transform, scale);

			shader.setVec3("viewPos", camera.Position);

			shader.setMat4("model", transform);
			shader.setMat4("view", view);
			shader.setMat4("projection", projection);

			drawFramebuffer(shader, meshIndex);
			
		}

		void renderFramebuffer(Avatar& avatar, Shader& shader, unsigned int meshIndex)
		{
			shader.use();

			glm::mat4 view = avatar.GetViewMatrix();
			glm::mat4 projection = avatar.GetProjectionMatrix();

			transform = glm::mat4(1.0f);

			transform = transform * physicsMatrix;
			transform = glm::translate(transform, glm::vec3(translate.x, translate.y, translate.z));

			transform = glm::rotate(transform, glm::radians(rotation.y), glm::vec3(0.0f, 1.0f, 0.0f));
			transform = glm::rotate(transform, glm::radians(rotation.z), glm::vec3(0.0f, 0.0f, 1.0f));
			transform = glm::rotate(transform, glm::radians(rotation.x), glm::vec3(1.0f, 0.0f, 0.0f));

			transform = glm::scale(transform, scale);

			shader.setVec3("viewPos", _glmFromOvrVector(avatar.position));

			shader.setMat4("model", transform);
			shader.setMat4("view", view);
			shader.setMat4("projection", projection);

			drawFramebuffer(shader, meshIndex);

		}

		void update() override
		{
			
			btTransform physicsTransform;
			body->getMotionState()->getWorldTransform(physicsTransform);
			physicsTransform.getOpenGLMatrix(glm::value_ptr(physicsMatrix));
			position = glm::vec3(physicsMatrix[3][0], physicsMatrix[3][1], physicsMatrix[3][2]);
			
		}

		void addBodyPhysicsMesh(int userIndex)
		{
			this->userIndex = userIndex;
			btTriangleMesh *trimesh = new btTriangleMesh();

			btVector3 scalar(shapeScalar.x, shapeScalar.y, shapeScalar.z);

			for (int j = 0; j < meshes.size(); j++)
			{
				vector<GLuint> indices = meshes[j].indices;
				vector<Vertex> vertex = meshes[j].vertices;

				for (int i = 0; i < indices.size() / 3; i++)
				{
					int index0 = indices[3 * i];
					int index1 = indices[3 * i + 1];
					int index2 = indices[3 * i + 2];

					btVector3 vertex0(vertex[index0].Position.x, vertex[index0].Position.y, vertex[index0].Position.z);
					btVector3 vertex1(vertex[index1].Position.x, vertex[index1].Position.y, vertex[index1].Position.z);
					btVector3 vertex2(vertex[index2].Position.x, vertex[index2].Position.y, vertex[index2].Position.z);

					trimesh->addTriangle(vertex0 * scalar, vertex1 * scalar, vertex2 * scalar);
				}
			}

			this->shape = new btConvexTriangleMeshShape(trimesh);
			bodyPhysicsConfiguration();
		}
		
		void addBodyPhysicsBox(int userIndex)
		{
			this->userIndex = userIndex;
			this->shape = new btBoxShape(btVector3(shapeScalar.x, shapeScalar.y, shapeScalar.z));
			bodyPhysicsConfiguration();
		}

		void addBodyPhysicsSphere(int userIndex)
		{
			this->userIndex = userIndex;
			this->shape = new btSphereShape(btScalar(shapeScalar.x));
			bodyPhysicsConfiguration();
		}

		void addBodyPhysicsCapsule(int userIndex)
		{
			this->userIndex = userIndex;
			this->shape = new btCapsuleShape(btScalar(shapeScalar.x), btScalar(shapeScalar.y));
			bodyPhysicsConfiguration();
		}

		void addBodyPhysicsCylinder(int userIndex)
		{
			this->userIndex = userIndex;
			this->shape = new btCylinderShape(btVector3(shapeScalar.x, shapeScalar.y, shapeScalar.z));
			bodyPhysicsConfiguration();
		}

		void addBodyPhysicsCone(int userIndex)
		{
			this->userIndex = userIndex;
			this->shape = new btConeShape(btScalar(shapeScalar.x), btScalar(shapeScalar.y));
			bodyPhysicsConfiguration();
		}

		void bodyPhysicsConfiguration()
		{

			BulletWorldController* worldController = BulletWorldController::Instance();

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
			body->setAngularFactor(btVector3(0, 0, 0));
			body->setGravity(btVector3(gravity.x, gravity.y, gravity.z));

			body->setActivationState(DISABLE_DEACTIVATION);

			//Añadimos el cuerpo al mundo dinámico
			worldController->dynamicsWorld->addRigidBody(body);

		}

		void changeBodyPhysics()
		{

			BulletWorldController::Instance()->dynamicsWorld->removeRigidBody(body);
			body = NULL;
			shape = NULL;

			switch (shape_current)
			{
			case 0:
				addBodyPhysicsBox(userIndex);
				break;
			case 1:
				addBodyPhysicsSphere(userIndex);
				break;
			case 2:
				addBodyPhysicsCapsule(userIndex);
				break;
			case 3:
				addBodyPhysicsCylinder(userIndex);
				break;
			case 4:
				addBodyPhysicsCone(userIndex);
				break;
			case 5:
				addBodyPhysicsMesh(userIndex);
				break;
			default:
				break;
			}						
		}

		void destroy()
		{
			BulletWorldController* worldController = BulletWorldController::Instance();
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
			body->setAngularFactor(btVector3(angularFactor[0] ? 1.0f : 0.0f, angularFactor[1] ? 1.0f : 0.0f, angularFactor[2] ? 1.0f : 0.0f));
		}

		void changeLinearVelocity() 
		{ 
			body->setLinearVelocity(btVector3(linearVelocity.x, linearVelocity.y, linearVelocity.z));
		}

		void changeMass()
		{
			body->setMassProps(mass, btVector3(localInertia.x, localInertia.y, localInertia.z));
			//shape->calculateLocalInertia(mass, btVector3(localInertia.x, localInertia.y, localInertia.z));
		}

		void changeScalar()
		{
			shape->setLocalScaling(btVector3(shapeScalar.x, shapeScalar.y, shapeScalar.z));
		}

	};
}
#endif