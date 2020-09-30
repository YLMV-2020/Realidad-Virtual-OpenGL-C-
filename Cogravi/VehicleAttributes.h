#ifndef VEHICLE_ATTRIBUTES_HEADER
#define VEHICLE_ATTRIBUTES_HEADER

namespace Cogravi
{
	class VehicleAttributes
	{
	public:

		btRaycastVehicle* car;

		btRigidBody* body;
		btCollisionShape* shape;



	};
}
#endif