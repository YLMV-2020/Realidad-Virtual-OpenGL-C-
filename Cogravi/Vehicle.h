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
			/*vector<Texture> textures;
			Texture t;


			t.id = Util::loadTexture("assets/objects/racing/auto.png");
			t.type = "texture_diffuse";

			textures.push_back(t);*/

			addBodyPhysicsBox(worldController);
			

			btRaycastVehicle::btVehicleTuning* tuning = new btRaycastVehicle::btVehicleTuning();

			btVehicleRaycaster* defvehicle = new btDefaultVehicleRaycaster(worldController->dynamicsWorld);

			car = new btRaycastVehicle(*tuning, body, defvehicle);
			car->setCoordinateSystem(0, 1, 2);


			btVector3 wheelDirection(0.0f, -1.0f, 0.0f);
			btVector3 wheelAxis(-1.0f, 0.0f, 0.0f);
			btScalar suspensionRestLength(0.2f);                                                                                                                 //TODO: PARAM
			btScalar wheelRadius(2.f);          
			//TOCO: PARAM
			car->addWheel(btVector3(-5.6f, 1.69f, 7.3), wheelDirection, wheelAxis, suspensionRestLength, wheelRadius, *tuning, true);   //TODO: PARAM
			car->addWheel(btVector3(3.f, 1.69f, 7.3), wheelDirection, wheelAxis, suspensionRestLength, wheelRadius, *tuning, true);    //TODO: PARAM
			car->addWheel(btVector3(-5.6f, 1.69f, -6.32f), wheelDirection, wheelAxis, suspensionRestLength, wheelRadius, *tuning, false);  //TODO: PARAM
			car->addWheel(btVector3(3.f, 1.69f, -6.32f), wheelDirection, wheelAxis, suspensionRestLength, wheelRadius, *tuning, false); //TODO: PARAM

			//car = vehicle;
			
			for (int i = 0; i < car->getNumWheels(); i++)
			{
				btWheelInfo& wheel = car->getWheelInfo(i);
				//wheel.m_wheelsDampingRelaxation = 1.7f;    //TODO: PARAM
				//wheel.m_wheelsDampingCompression = -1.7f;   //TODO: PARAM
				//wheel.m_frictionSlip = btScalar(10000.); //TODO: PARAM
				//wheel.m_rollInfluence = btScalar(0.f);   //TODO: PARAM
				//wheel.m_maxSuspensionTravelCm = 100.f;   //TODO: PARAM
			}
			
			car->debugDraw(worldController->dynamicsWorld->getDebugDrawer());
			//worldController->dynamicsWorld->addAction(car);
			worldController->dynamicsWorld->addVehicle(car);
		}


		void addBodyPhysicsBox(BulletWorldController* worldController)
		{
			btCollisionShape* nshape = new btBoxShape(btVector3(1.0f, 1.0f, 1.0f));
			this->shape = nshape;
			bodyPhysicsConfiguration(worldController);
		}

		void bodyPhysicsConfiguration(BulletWorldController* worldController)
		{
			btTransform transform;
			transform.setIdentity();
			transform.setOrigin(btVector3(-30, 25.0f, 15.f));
			transform.setRotation(btQuaternion(0, 0, 0, 1));

			//Calculamos la inercia del modelo
			btVector3 inertia(1, 1, 1);
			shape->calculateLocalInertia(1, inertia);

			//Configuramos las propiedades básicas de construcción del cuerpo

			btDefaultMotionState* state = new btDefaultMotionState(transform);
			btRigidBody::btRigidBodyConstructionInfo info(1, state, shape, inertia);

			//Establecemos los parámetros que recibidos como parámetro
			body = new btRigidBody(info);
			//body->setRestitution(1);
			//body->setUserIndex(200);
			body->setLinearFactor(btVector3(1, 1, 1));

			//body->setSleepingThresholds(0.0, 0.0);
			//body->setAngularFactor(0.0);

			//Por defecto, todos los modelos están bloqueados en el espacio en X,Z así como sus ejes de rotación
			body->setAngularFactor(btVector3(0, 1, 0));
			body->setGravity(btVector3(0, 1, 0));

			body->setActivationState(DISABLE_DEACTIVATION);

			//Añadimos el cuerpo al mundo dinámico
			worldController->dynamicsWorld->addRigidBody(body);

		}

		

		void update()
		{
			if (car->getSteeringValue(0) > 0)
			{
				car->setSteeringValue(car->getSteeringValue(0) - 0.030f, 0); //TODO: PARAM
				car->setSteeringValue(car->getSteeringValue(1) - 0.030f, 1); //TODO: PARAM
				if (car->getSteeringValue(0) < 0)
				{
					car->setSteeringValue(0, 0);
					car->setSteeringValue(0, 1);
				}
			}
			else if (car->getSteeringValue(0) < 0)
			{
				car->setSteeringValue(car->getSteeringValue(0) + 0.03f, 0); //TODO: PARAM
				car->setSteeringValue(car->getSteeringValue(1) + 0.03f, 1); //TODO: PARAM
				if (car->getSteeringValue(0) > 0)
				{
					car->setSteeringValue(0, 0);
					car->setSteeringValue(0, 1);
				}
			}
		}

		void render(Camera &camera)
		{
			
		}

	};
}
#endif