#ifndef VEHICLE_HEADER
#define VEHICLE_HEADER

#include "VehicleAttributes.h"

namespace Cogravi
{
	class Vehicle :public VehicleAttributes
	{
	public:

		Vehicle(BulletWorldController *worldController)
		{
			vector<Texture> textures;
			Texture t;


			t.id = Util::loadTexture("assets/objects/racing/auto.png");
			t.type = "texture_diffuse";

			textures.push_back(t);

			//Modelo [0] del vehiculo: Chasis
			vehicleDrawableModels.push_back(new Model(
				glm::vec3(position.x, position.y, position.z) ,
				glm::vec3(0.00f, 0.00f, 0.00f),
				glm::vec3(0.50f, 0.50f, 0.50f),
				"assets/objects/racing/auto.obj",
				textures, *Util::Instance()->myShaders[ShaderType::MODEL_STATIC])
			);

			vehicleDrawableModels[0]->mass = 2000.0f;
			vehicleDrawableModels[0]->restitution = 0.5f;
			vehicleDrawableModels[0]->linearFactor = glm::vec3(1, 1, 1);
			//Configuración física del chasis
			vehicleDrawableModels[0]->addBodyPhysicsBox(100, worldController);
			vehicleDrawableModels[0]->body->setAngularFactor(btVector3(1, 1, 1));
			vehicleDrawableModels[0]->body->setLinearFactor(btVector3(1, 1, 1));
			vehicleDrawableModels[0]->body->setActivationState(DISABLE_DEACTIVATION);

			textures.clear();

			t.id = Util::loadTexture("assets/objects/racing/rueda.png");
			t.type = "texture_diffuse";

			textures.push_back(t);

			//Modelo [1] del vehiculo: Rueda FL
			vehicleDrawableModels.push_back(new Model(
				glm::vec3(position.x - 3.f, position.y - 2.f, position.z - 3.f) ,
				glm::vec3(0.00f, 0.00f, 0.00f),
				glm::vec3(1.00f, 1.00f, 1.00f),
				"assets/objects/racing/rueda.obj",
				textures, * Util::Instance()->myShaders[ShaderType::MODEL_STATIC])
			);

			//Configuración física de la rueda FL
			vehicleDrawableModels[1]->mass = 10.0f;
			vehicleDrawableModels[1]->restitution = 0.5f;
			vehicleDrawableModels[1]->linearFactor = glm::vec3(1, 1, 1);


			vehicleDrawableModels[1]->addBodyPhysicsSphere(101, worldController);
			vehicleDrawableModels[1]->body->setAngularFactor(btVector3(1, 1, 1));
			vehicleDrawableModels[1]->body->setGravity(btVector3(1, 1, 1));


			//Modelo [2] del vehiculo: Rueda FR
			vehicleDrawableModels.push_back(new Model(

				glm::vec3(position.x - 3.f, position.y - 2.f, position.z + 3.f) ,
				glm::vec3(0.00f, 0.00f, 0.00f),
				glm::vec3(1.00f, 1.00f, 1.00f),
				"assets/objects/racing/rueda.obj",
				textures, * Util::Instance()->myShaders[ShaderType::MODEL_STATIC])
			
			);

			//Configuración física de la rueda FR

			vehicleDrawableModels[2]->mass = 10.0f;
			vehicleDrawableModels[2]->restitution = 0.5f;
			vehicleDrawableModels[2]->linearFactor = glm::vec3(1, 1, 1);

			vehicleDrawableModels[2]->addBodyPhysicsSphere(102, worldController);
			vehicleDrawableModels[2]->body->setAngularFactor(btVector3(1, 1, 1));
			vehicleDrawableModels[2]->body->setGravity(btVector3(1, 1, 1));

			//Modelo [3] del vehiculo: Rueda BL
			vehicleDrawableModels.push_back(new Model(
				glm::vec3(position.x + 3.f, position.y - 2.f, position.z - 3.f),
				glm::vec3(0.00f, 0.00f, 0.00f),
				glm::vec3(1.00f, 1.00f, 1.00f),
				"assets/objects/racing/rueda.obj",
				textures, * Util::Instance()->myShaders[ShaderType::MODEL_STATIC])		
			);

			vehicleDrawableModels[3]->mass = 10.0f;
			vehicleDrawableModels[3]->restitution = 0.5f;
			vehicleDrawableModels[3]->linearFactor = glm::vec3(1, 1, 1);
			//Configuración física de la rueda BL
			vehicleDrawableModels[3]->addBodyPhysicsSphere(103, worldController);
			vehicleDrawableModels[3]->body->setAngularFactor(btVector3(1, 1, 1));
			vehicleDrawableModels[3]->body->setGravity(btVector3(1, 1, 1));

			//Modelo [4] del vehiculo: Rueda BR
			vehicleDrawableModels.push_back(new Model(

				glm::vec3(position.x + 3.f, position.y - 2.f, position.z + 3.f),
				glm::vec3(0.00f, 0.00f, 0.00f),
				glm::vec3(1.00f, 1.00f, 1.00f),
				"assets/objects/racing/rueda.obj",
				textures, * Util::Instance()->myShaders[ShaderType::MODEL_STATIC])
			);

			vehicleDrawableModels[4]->mass = 10.0f;
			vehicleDrawableModels[4]->restitution = 0.5f;
			vehicleDrawableModels[4]->linearFactor = glm::vec3(1, 1, 1);
			//Configuración física de la rueda BR
			vehicleDrawableModels[4]->addBodyPhysicsSphere(104, worldController);
			vehicleDrawableModels[4]->body->setAngularFactor(btVector3(1, 1, 1));
			vehicleDrawableModels[4]->body->setGravity(btVector3(1, 1, 1));

			//Creamos la "Hinge" Catapulta, que unirá el brazo móvil con el chasis del vehiculo

			//Ajustamos los ejes de funcionamiento de la catapulta
			btVector3 childAxis(0.f, 0.f, 1.f);
			btVector3 parentAxis(0.f, 0.f, 1.f);



			//Creamos las uniones de cada una de las ruedas con el chasis del vehiculo
			//y las configuramos
			for (int i = 1; i <= 4; i++)
			{
				//Configuramos los parámetros de cada rueda
				vehicleDrawableModels[i]->body->setActivationState(DISABLE_DEACTIVATION);
				vehicleDrawableModels[i]->body->setFriction(1000.f);

				//Calculamos el punto de anclaje del hinge según la posición de cada rueda
				btTransform anchor_transform;
				anchor_transform.setIdentity();

				anchor_transform.setOrigin(btVector3(
					vehicleDrawableModels[i]->position.x,
					vehicleDrawableModels[i]->position.y,
					vehicleDrawableModels[i]->position.z)
				);

				//Configuramos los ejes de funcionamiento y ajustamos el punto de anclaje
				btVector3 parentAxis(0.f, 1.f, 0.f);
				btVector3 childAxis(0.f, 0.f, 1.f);
				btVector3 anchor = anchor_transform.getOrigin();

				//Creamos la unión y la añadimos al vector de Engines(para poder interactuar con cada motor fuera de la clase)
				btHinge2Constraint* wheel_hinge = new btHinge2Constraint(*vehicleDrawableModels[0]->body, *vehicleDrawableModels[i]->body, anchor, parentAxis, childAxis);
				vehicle_Engines.push_back(wheel_hinge);

				//Añadimos la unión al mundo
				worldController->dynamicsWorld->addConstraint(wheel_hinge, true);

				//Configuración del motor de fuerza (Drive)
				wheel_hinge->enableMotor(3, true);
				wheel_hinge->setMaxMotorForce(3, wheelEngineForce);
				wheel_hinge->setTargetVelocity(3, 0);

				//Configuración del motor de dirección (Steer)
				wheel_hinge->enableMotor(5, true);
				wheel_hinge->setMaxMotorForce(5, wheelEngineForce);
				wheel_hinge->setTargetVelocity(5, 0);
				wheel_hinge->setDamping(2, 2.0);
				wheel_hinge->setStiffness(2, 4.0);
			}
		}

		//Configuración de la velocidad de aceleración de las ruedas
		void set_drive_engine_vel(float velocity)
		{
			vehicle_Engines[0]->setTargetVelocity(3, velocity);
			vehicle_Engines[1]->setTargetVelocity(3, velocity);
			vehicle_Engines[2]->setTargetVelocity(3, velocity);
			vehicle_Engines[3]->setTargetVelocity(3, velocity);
		}

		//Configuración del ángulo de dirección de las ruedas para el giro de la catapulta
		void set_steer_engine_vel(float velocity)
		{
			vehicle_Engines[0]->setTargetVelocity(5, velocity);
			vehicle_Engines[1]->setTargetVelocity(5, velocity);
			vehicle_Engines[2]->setTargetVelocity(5, -velocity);
			vehicle_Engines[3]->setTargetVelocity(5, -velocity);
		}

		void update()
		{
			for (Model*& model : vehicleDrawableModels)
				model->update();
		}

		void render(Camera &camera)
		{
			for (Model*& model : vehicleDrawableModels)
				model->render(camera);
		}

	};
}
#endif