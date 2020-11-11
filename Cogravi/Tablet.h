#ifndef TABLET_HEADER
#define TABLET_HEADER

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h> 

#define TABLET_ID 2020

#include "MeshM.h"

namespace Cogravi {

	class Tablet
	{
	public:
		
		static Tablet* Instance()
		{
			static Tablet instance;
			return &instance;
		}

		Model* model;

		Tablet()
		{
			ModelController::Instance()->addModel(glm::vec3(20.0f, 0.0f, 20.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(10.f, 10.f, 10.f), "tablet/tablet.obj", ColliderType::BOX, BulletWorldController::Instance(), glm::vec3(0.63f, 0.94f, 0.09f), glm::vec3(0.0f, -0.95f, 0.0f));
			model = ModelController::Instance()->getModelPhysics(0);
			
			//model->meshes.erase(model->meshes.begin());

			//cout << "Mesh 1: " << model->meshes[0].textures.size() << "\n";
			//model->meshes[0].
			
			//cout << "Mesh 1: " << model->meshes[0].textures.size() << "\n";
	
		}

		void updateTexture(GLuint texture)
		{
			model->meshes[2].textures[0].id = texture;
		}

		void render(Camera& camera, Shader& shader)
		{
			model->renderFramebuffer(camera, shader, 2);
		}


	};
}
#endif