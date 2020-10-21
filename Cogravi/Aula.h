#ifndef AULA_HEADER
#define AULA_HEADER

namespace Cogravi 
{
    class Aula: public GameObject
    {

    public:

        vector<btRigidBody*> body;
        vector<btCollisionShape*> shape;
		BulletWorldController* worldController;

		glm::vec3 shapeScalar = glm::vec3(1.0f);
		int userIndex;

        /*static Aula* Instance()
        {
            static Aula instance;
            return &instance;
        }*/

        Aula(glm::vec3 position, glm::vec3 rotation, glm::vec3 scale, string const& path, BulletWorldController* worldController, vector<Texture>textures = {}) :
            GameObject(position, rotation, scale, path, textures)
        {
			this->worldController = worldController;
			addBodyPhysicsBox(worldController, glm::vec3(13.50f, 0.0f, 0.f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.1f, 3.40f, 8.50f));
			addBodyPhysicsBox(worldController, glm::vec3(-13.50f, 0.0f, 0.f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.1f, 3.40f, 8.50f));
			addBodyPhysicsBox(worldController, glm::vec3(0.0f, 0.0f, -9.0f), glm::vec3(0.0f, 90.0f, 0.0f), glm::vec3(0.1f, 3.40f, 13.5f));
			//addBodyPhysicsBox(worldController, glm::vec3(-13.50f, 0.0f, 15.0f), glm::vec3(0.1f, 3.40f, 8.0f));
        }

		void addBodyPhysicsBox(BulletWorldController* worldController, glm::vec3 traslation, glm::vec3 rotate, glm::vec3 size)
        {
			this->userIndex = shape.size() + 101;;
			this->shape.push_back(new btBoxShape(btVector3(size.x, size.y, size.z)));
            bodyPhysicsConfiguration(worldController, traslation, rotate);
        }

		void bodyPhysicsConfiguration(BulletWorldController* worldController, glm::vec3 traslation, glm::vec3 rotate)
		{
			btTransform transform;
			transform.setIdentity();
			transform.setOrigin(btVector3(position.x + traslation.x, position.y + traslation.y, position.z + traslation.z));

			btQuaternion quat;
			quat.setEulerZYX(btScalar(glm::radians(rotate.z + rotation.z)), btScalar(glm::radians(rotate.y + rotation.y)), btScalar(glm::radians(rotate.x + rotation.x)));
			transform.setRotation(btQuaternion(quat));

			//Calculamos la inercia del modelo
			btVector3 inertia(0, 0, 0);
			shape.back()->calculateLocalInertia(10.0f, inertia);

			//Configuramos las propiedades básicas de construcción del cuerpo

			btDefaultMotionState* state = new btDefaultMotionState(transform);
			btRigidBody::btRigidBodyConstructionInfo info(10, state, shape.back(), inertia);

			//Establecemos los parámetros que recibidos como parámetro
			body.push_back(new btRigidBody(info));
			body.back()->setRestitution(0.0f);
			body.back()->setLinearFactor(btVector3(1, 1, 1));
			body.back()->setUserIndex(this->userIndex);

			//Por defecto, todos los modelos están bloqueados en el espacio en X,Z así como sus ejes de rotación
			body.back()->setAngularFactor(btVector3(0, 0, 0));
			body.back()->setGravity(btVector3(0, 1, 0));

			//body.back()->setActivationState(DISABLE_DEACTIVATION);

			//Añadimos el cuerpo al mundo dinámico
			worldController->dynamicsWorld->addRigidBody(body.back());

		}

		void changeScalar(btCollisionShape *shape)
		{
			shape->setLocalScaling(btVector3(shapeScalar.x, shapeScalar.y, shapeScalar.z));
		}
      
    };
}
#endif
