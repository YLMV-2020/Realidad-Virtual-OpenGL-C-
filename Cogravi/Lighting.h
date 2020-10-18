#ifndef LUZ_HEADER
#define LUZ_HEADER

namespace Cogravi
{

	glm::vec3 pointLightPositions[] = {
		glm::vec3(0.7f,  0.2f,  2.0f),
		glm::vec3(2.3f, -3.3f, -4.0f),
		glm::vec3(-4.0f,  2.0f, -12.0f),
		glm::vec3(0.0f,  0.0f, -3.0f)
	};


	glm::vec3 pointLightColors[] = {
		glm::vec3(0.2f, 0.2f, 0.6f),
		glm::vec3(0.3f, 0.3f, 0.7f),
		glm::vec3(0.0f, 0.0f, 0.3f),
		glm::vec3(0.4f, 0.4f, 0.4f)
	};

	struct LuzDireccional
	{
		glm::vec3 direction;
		glm::vec3 ambient;
		glm::vec3 diffuse;
		glm::vec3 specular;
	};

	struct LuzPuntual
	{
		glm::vec3 position;
		glm::vec3 ambient;
		glm::vec3 diffuse;
		glm::vec3 specular;
		float constant;
		float linear;
		float quadratic;
	};

	struct LuzFocal
	{
		glm::vec3 position;
		glm::vec3 direction;
		glm::vec3 ambient;
		glm::vec3 diffuse;
		glm::vec3 specular;
		float constant;
		float linear;
		float quadratic;
		float cutOff;
		float outerCutOff;
	};

	class Lighting
	{
	private:

		vector<GameObject*> sol;

	public:

		LuzDireccional luzDireccional;
		vector<LuzPuntual> luzPuntual;
		LuzFocal luzFocal;


		static Lighting* Instance()
		{
			static Lighting instance;
			return &instance;
		}

		Lighting()
		{
			luzDireccional.ambient = glm::vec3(1.0f, 1.0f, 1.0f);
			luzDireccional.diffuse = glm::vec3(1.0f, 1.0f, 1.0f);
			luzDireccional.specular = glm::vec3(1.0f, 1.0f, 1.0f);

			luzFocal.ambient = glm::vec3(0.0f, 0.0f, 0.0f);
			luzFocal.diffuse = glm::vec3(1.0f, 1.0f, 1.0f);
			luzFocal.specular = glm::vec3(1.0f, 1.0f, 1.0f);
			luzFocal.constant = 1.0f;
			luzFocal.linear = 0.009f;
			luzFocal.quadratic = 0.0032f;
			luzFocal.cutOff = 10.0f;
			luzFocal.outerCutOff = 12.5f;

		}

		void addSol(glm::vec3 position, glm::vec3 rotation, glm::vec3 scale)
		{
			vector<Texture> texturas;
			texturas.push_back(Texture(Util::loadTexture("assets/objects/sol/texture sol.png"), TextureType::DIFFUSE));
			GameObject* model = new GameObject(position, rotation, scale, "assets/objects/sol/sol.obj", texturas);
			sol.push_back(model);

			LuzPuntual luz;

			luz.position = glm::vec3(position.x, position.y, position.z);
			luz.ambient = glm::vec3(0.0f, 0.0f, 0.0f);
			luz.diffuse = glm::vec3(1.0f, 1.0f, 1.0f);
			luz.specular = glm::vec3(1.0f, 1.0f, 1.0f);
			luz.constant = 1.0f;
			luz.linear = 0.09f;
			luz.quadratic = 0.032f;

			luzPuntual.push_back(luz);
		}

		void deleteSol(int index)
		{
			if (sol.size() > 0)
			{
				sol.erase(sol.begin() + index);
				luzPuntual.erase(luzPuntual.begin() + index);
			}
		}

		//void updateSol(int index, glm::vec3 ambient)
		//{
		//	sol[index]->shader.use();
		//	sol[index]->shader.setVec3("color", ambient);
		//}

		void render(Camera& camera, Shader& shader)
		{
			for (GameObject*& model : sol)
				model->render(camera, shader);
		}

		int sizeSol()
		{
			return sol.size();
		}

		GameObject* getSol(int index)
		{
			return sol[index];
		}


	};
}
#endif