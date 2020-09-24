#ifndef BULLET_WORLD_CONTROLLER_HEADER
#define BULLET_WORLD_CONTROLLER_HEADER

#include <btBulletDynamicsCommon.h>
#include <memory>
#include <vector>

using namespace std;
namespace Cogravi
{
	class BulletWorldController
	{

	public:

		btDiscreteDynamicsWorld * dynamicsWorld; //Mundo físico para bullet

		//Vectores a punteros de los elementos de Bullet
		//vector<btRigidBody*> rigidBodies;
		//vector<btDefaultMotionState*> motionStates;
		//vector<btCollisionShape*> collisionShapes;

		//Parámetros de configuración del mundo Bullet
		btDefaultCollisionConfiguration     collisionConfiguration;
		btCollisionDispatcher               collisionDispatcher;
		btDbvtBroadphase                    overlappingPairCache;
		btSequentialImpulseConstraintSolver constraintSolver;

		//Creamos y configuramos el mundo físico en el constructor
		BulletWorldController() : collisionDispatcher(&collisionConfiguration)
		{
			btDiscreteDynamicsWorld* dynamicsWorld
			(
				new btDiscreteDynamicsWorld
				(
					&collisionDispatcher,
					&overlappingPairCache,
					&constraintSolver,
					&collisionConfiguration
				)
			);

			dynamicsWorld->setGravity(btVector3(0, -9.8f, 0));

			this->dynamicsWorld = dynamicsWorld;
		}

		//Actualizamos la física
		void physics_step(float Hz)
		{
			dynamicsWorld->stepSimulation(1.f / Hz);
		}

		//Comprobación de colisiones, devuelve true si están colisionando dos objetos con las IDs indicadas
		bool check_collisions(int id_1, int id_2)
		{
			int numManifolds = dynamicsWorld->getDispatcher()->getNumManifolds();
			for (int i = 0; i < numManifolds; i++)
			{
				btPersistentManifold* contactManifold = dynamicsWorld->getDispatcher()->getManifoldByIndexInternal(i);
				btCollisionObject* obA = (btCollisionObject*)(contactManifold->getBody0());
				btCollisionObject* obB = (btCollisionObject*)(contactManifold->getBody1());

				if ((obA->getUserIndex() == id_1 && obB->getUserIndex() == id_2) || (obA->getUserIndex() == id_2 && obB->getUserIndex() == id_1))
				{
					return true;
				}
			}
			return false;
		}

		// trazado de rayos
		void screenPosToWorldRay(
			int mouseX, int mouseY,             // Mouse position, in pixels, from bottom-left corner of the window
			int screenWidth, int screenHeight,  // Window size, in pixels
			glm::mat4 ViewMatrix,               // Camera position and orientation
			glm::mat4 ProjectionMatrix,         // Camera parameters (ratio, field of view, near and far planes)
			glm::vec3& out_origin,              // Ouput : Origin of the ray. /!\ Starts at the near plane, so if you want the ray to start at the camera's position instead, ignore this.
			glm::vec3& out_direction            // Ouput : Direction, in world space, of the ray that goes "through" the mouse.
		) {

			// The ray Start and End positions, in Normalized Device Coordinates (Have you read Tutorial 4 ?)
			glm::vec4 lRayStart_NDC(
				((2.0f * (float)mouseX) / (float)screenWidth - 1), // [0,1024] -> [-1,1]
				((2.0f * (float)mouseY) / (float)screenHeight - 1), // [0, 768] -> [-1,1]
				-1.0, // The near plane maps to Z=-1 in Normalized Device Coordinates
				1.0f
			);
			glm::vec4 lRayEnd_NDC(
				((2.0f * (float)mouseX) / (float)screenWidth - 1), // [0,1024] -> [-1,1]
				((2.0f * (float)mouseY) / (float)screenHeight - 1), // [0, 768] -> [-1,1]
				0.0,
				1.0f
			);


			// The Projection matrix goes from Camera Space to NDC.
			// So inverse(ProjectionMatrix) goes from NDC to Camera Space.
			glm::mat4 InverseProjectionMatrix = glm::inverse(ProjectionMatrix);

			// The View Matrix goes from World Space to Camera Space.
			// So inverse(ViewMatrix) goes from Camera Space to World Space.
			glm::mat4 InverseViewMatrix = glm::inverse(ViewMatrix);

			glm::vec4 lRayStart_camera = InverseProjectionMatrix * lRayStart_NDC;    lRayStart_camera /= lRayStart_camera.w;
			glm::vec4 lRayStart_world = InverseViewMatrix * lRayStart_camera; lRayStart_world /= lRayStart_world.w;
			glm::vec4 lRayEnd_camera = InverseProjectionMatrix * lRayEnd_NDC;      lRayEnd_camera /= lRayEnd_camera.w;
			glm::vec4 lRayEnd_world = InverseViewMatrix * lRayEnd_camera;   lRayEnd_world /= lRayEnd_world.w;


			// Faster way (just one inverse)
			//glm::mat4 M = glm::inverse(ProjectionMatrix * ViewMatrix);
			//glm::vec4 lRayStart_world = M * lRayStart_NDC; lRayStart_world/=lRayStart_world.w;
			//glm::vec4 lRayEnd_world   = M * lRayEnd_NDC  ; lRayEnd_world  /=lRayEnd_world.w;

			glm::vec3 lRayDir_world(lRayEnd_world - lRayStart_world);
			lRayDir_world = glm::normalize(lRayDir_world);

			out_origin = glm::vec3(lRayStart_world);
			out_direction = glm::normalize(lRayDir_world);
		}

	};
}

#endif