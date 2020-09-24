#ifndef VEHICLE_ATTRIBUTES_HEADER
#define VEHICLE_ATTRIBUTES_HEADER

namespace Cogravi
{
	class VehicleAttributes
	{
	public:

		glm::vec3 position = glm::vec3(5.0f, 5.0f, 5.0f);

		//Fuerza de los motores que componen un vehiculo
		float catapultEngineForce = 15.f;
		float wheelEngineForce = 10000.f;

		std::vector<btHinge2Constraint*> vehicle_Engines; //Vector con los 4 motores de las ruedas
		std::vector<Model*> vehicleDrawableModels; //Vector con todos los modelos del vehiculo
		



	};
}
#endif