#ifndef VEHICLE_CONTROLLER_HEADER
#define VEHICLE_CONTROLLER_HEADER

#include "Vehicle.h"

namespace Cogravi
{
	class VehicleController
	{
	public:

		Vehicle* vehicle;

		VehicleController(BulletWorldController *worldController)
		{
			vehicle = new Vehicle(worldController);
		}

		void update()
		{
			vehicle->update();
		}

		void render(Camera& camera)
		{
			vehicle->render(camera);
		}

		void updateInput(GLFWwindow* window)
		{
			if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS)
			{
				cout << "Km: " << vehicle->car->getCurrentSpeedKmHour() << "\n";
				vehicle->car->getRigidBody()->applyCentralImpulse(btVector3(0, 1, 0));
				if (vehicle->car->getCurrentSpeedKmHour() < 150.f)
				{
					//vehicle->car->setBrake(20, 1);
					//vehicle->car->getRigidBody()->applyCentralImpulse(btVector3(0, 0, 30));
					vehicle->car->applyEngineForce(-20, 0);
					vehicle->car->applyEngineForce(-20, 1);
				}


			}
			else if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS)
			{
				
			}
			else
			{
				
			}

			if (glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS)
			{
				
			}
			else if (glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS)
			{
				
			}
			else
			{
				
			}
			
		}

	};
}
#endif
