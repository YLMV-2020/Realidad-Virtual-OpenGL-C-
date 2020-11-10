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

		int numPuntual;
		int shininess;

		bool isLightDirectional = false;
		bool isLightPoint = false;
		bool isLightSpot = false;

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

			shininess = 32.0f;
			numPuntual = 0;
		}

		void loadShader(Camera& camera, Shader& shader, Shader& shaderSol)
		{
			shader.use();

			shader.setVec3("viewPos", camera.Position);

			shader.setBool("isLightDirectional", isLightDirectional);
			shader.setBool("isLightPoint", isLightPoint);
			shader.setBool("isLightSpot", isLightSpot);
			shader.setFloat("material.shininess", shininess);

			if (isLightDirectional)
			{
				shader.setVec3("dirLight.direction", luzDireccional.direction);
				shader.setVec3("dirLight.ambient", luzDireccional.ambient);
				shader.setVec3("dirLight.diffuse", luzDireccional.diffuse);
				shader.setVec3("dirLight.specular", luzDireccional.specular);
			}

			if (isLightSpot)
			{
				shader.setVec3("spotLight.position", camera.Position);
				shader.setVec3("spotLight.direction", camera.Front);
				shader.setVec3("spotLight.ambient", luzFocal.ambient);
				shader.setVec3("spotLight.diffuse", luzFocal.diffuse);
				shader.setVec3("spotLight.specular", luzFocal.specular);
				shader.setFloat("spotLight.constant", luzFocal.constant);
				shader.setFloat("spotLight.linear", luzFocal.linear);
				shader.setFloat("spotLight.quadratic", luzFocal.quadratic);
				shader.setFloat("spotLight.cutOff", glm::cos(glm::radians(luzFocal.cutOff)));
				shader.setFloat("spotLight.outerCutOff", glm::cos(glm::radians(luzFocal.outerCutOff)));
			}

			if (isLightPoint)
			{
				shader.setInt("sizePointLights", numPuntual);

				for (int i = 0; i < numPuntual; i++)
				{
					updateSol(shaderSol, luzPuntual[i].ambient);
					loadShaderLightPoint(shader, i);
					sol[i]->render(camera, shaderSol);
				}
			} 	
		}

		void loadShaderLightPoint(Shader& shader, int index)
		{
			shader.use();

			shader.setVec3("pointLights[" + to_string(index) + "].position", getSol(index)->position);
			shader.setVec3("pointLights[" + to_string(index) + "].ambient", luzPuntual[index].ambient);
			shader.setVec3("pointLights[" + to_string(index) + "].diffuse", luzPuntual[index].diffuse);
			shader.setVec3("pointLights[" + to_string(index) + "].specular", luzPuntual[index].specular);
			shader.setFloat("pointLights[" + to_string(index) + "].constant", luzPuntual[index].constant);
			shader.setFloat("pointLights[" + to_string(index) + "].linear", luzPuntual[index].linear);
			shader.setFloat("pointLights[" + to_string(index) + "].quadratic", luzPuntual[index].quadratic);			
		}

		void addSol(glm::vec3 position, glm::vec3 rotation, glm::vec3 scale)
		{
			GameObject* model = new GameObject(position, rotation, scale, "assets/objects/cubo.obj");
			sol.push_back(model);

			LuzPuntual luz;

			luz.position = glm::vec3(position.x, position.y, position.z);
			luz.ambient = glm::vec3(0.0f, 0.0f, 0.0f);
			luz.diffuse = glm::vec3(1.0f, 1.0f, 1.0f);
			luz.specular = glm::vec3(1.0f, 1.0f, 1.0f);
			luz.constant = 1.0f;
			luz.linear = 0.09f;
			luz.quadratic = 0.032f;

			numPuntual++;

			luzPuntual.push_back(luz);
		}

		void deleteSol(int index)
		{
			if (sol.size() > 0)
			{
				sol.erase(sol.begin() + index);
				luzPuntual.erase(luzPuntual.begin() + index);
				numPuntual--;
			}
			
		}

		void updateSol(Shader& shader, glm::vec3 ambient)
		{
			shader.use();
			shader.setVec3("color", ambient);
		}

		void render(Camera& camera, Shader& shader)
		{
			for (GameObject*& model : sol)
				model->render(camera, shader);	
		}

		int sizeSol()
		{
			return numPuntual;
		}

		GameObject* getSol(int index)
		{
			return sol[index];
		}


	};
}
#endif