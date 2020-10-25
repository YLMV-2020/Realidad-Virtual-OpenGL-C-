#ifndef LUZ_HEADER
#define LUZ_HEADER

namespace Cogravi
{


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
		int shininess;


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

			shininess = 5;
		}

		void loadShader(Shader& shader)
		{
			shader.setBool("isLightDirectional", true);
			shader.setBool("isLightPoint", true);
			shader.setBool("isLightSpot", true);
			shader.setFloat("material.shininess", 32.0f);

			shader.setVec3("dirLight.direction", luzDireccional.direction);
			shader.setVec3("dirLight.ambient", luzDireccional.ambient);
			shader.setVec3("dirLight.diffuse", luzDireccional.diffuse);
			shader.setVec3("dirLight.specular", luzDireccional.specular);
                

			shader.setVec3("spotLight.ambient", luzFocal.ambient);
			shader.setVec3("spotLight.diffuse", luzFocal.diffuse);
			shader.setVec3("spotLight.specular", luzFocal.specular);
			shader.setFloat("spotLight.constant", luzFocal.constant);
			shader.setFloat("spotLight.linear", luzFocal.linear);
			shader.setFloat("spotLight.quadratic", luzFocal.quadratic);
			shader.setFloat("spotLight.cutOff", glm::cos(glm::radians(luzFocal.cutOff)));
			shader.setFloat("spotLight.outerCutOff", glm::cos(glm::radians(luzFocal.outerCutOff)));
                    
		
		}

		void loadShaderLightPoint(Shader& shader)
		{
			int i = sizeSol() - 1;
			shader.setVec3("pointLights[" + to_string(i) + "].position", getSol(i)->position);
			shader.setVec3("pointLights[" + to_string(i) + "].ambient", luzPuntual[i].ambient);
			shader.setVec3("pointLights[" + to_string(i) + "].diffuse", luzPuntual[i].diffuse);
			shader.setVec3("pointLights[" + to_string(i) + "].specular", luzPuntual[i].specular);
			shader.setFloat("pointLights[" + to_string(i) + "].constant", luzPuntual[i].constant);
			shader.setFloat("pointLights[" + to_string(i) + "].linear", luzPuntual[i].linear);
			shader.setFloat("pointLights[" + to_string(i) + "].quadratic", luzPuntual[i].quadratic);			
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