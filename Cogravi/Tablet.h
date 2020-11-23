#ifndef TABLET_HEADER
#define TABLET_HEADER

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h> 

#include "MeshM.h"

namespace Cogravi {

	class Tablet
	{
	public:
		
		const unsigned int ID = 0;

		static Tablet* Instance()
		{
			static Tablet instance;
			return &instance;
		}

		Model* model;

		Tablet()
		{
			ModelController::Instance()->addModel(glm::vec3(0.0f, 0.0f, 30.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(1.f, 1.f, 1.f), "tablet/tablet.obj", ColliderType::BOX, glm::vec3(0.63f, 0.94f, 0.09f), glm::vec3(0.0f, -0.95f, 0.0f));
			model = ModelController::Instance()->getModelPhysics(ID);
		}

		void updateTexture(GLuint texture)
		{
			model->meshes[2].textures[0].id = texture;
		}

		void render(Camera& camera, Shader& shader)
		{
			model->renderFramebuffer(camera, shader, 2);
		}

		void render(Avatar& avatar, Shader& shader)
		{
			model->renderFramebuffer(avatar, shader, 2);
		}

	};
}
#endif