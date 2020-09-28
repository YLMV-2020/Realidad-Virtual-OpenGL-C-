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
				vehicle->set_drive_engine_vel(-5);
			}
			else if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS)
			{
				vehicle->set_drive_engine_vel(5);
			}
			else
			{
				vehicle->set_drive_engine_vel(0);
			}

			if (glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS)
			{
				vehicle->set_steer_engine_vel(1);
			}
			else if (glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS)
			{
				vehicle->set_steer_engine_vel(-1);
			}
			else
			{
				vehicle->set_steer_engine_vel(0);
			}
			
		}

	};
}
#endif
