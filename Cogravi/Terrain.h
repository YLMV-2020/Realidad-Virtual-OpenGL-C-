#ifndef TERRAIN_HEADER
#define TERRAIN_HEADER

namespace Cogravi
{
	class Terrain
	{
	public:

		btStaticPlaneShape * plane;
		Shader shader;

		unsigned int floorTexture;
		unsigned int VAO, VBO;

		glm::vec3 volumen;
		float repeat;

		float planeVertices[30];

		Terrain(string path, glm::vec3 volumen, float repeat)
		{
			this->shader = *Util::Instance()->myShaders[ShaderType::TERRAIN];
			this->volumen = volumen;
			this->repeat = repeat;

			btTransform transform;
			transform.setIdentity();
			transform.setOrigin(btVector3(0, 0, 0));

			plane = new btStaticPlaneShape(btVector3(0, 1, 0), 0);
			btMotionState* motion = new btDefaultMotionState(transform);
			btRigidBody::btRigidBodyConstructionInfo info(0.0, motion, plane);

			btRigidBody* body = new btRigidBody(info);

			BulletWorldController::Instance()->dynamicsWorld->addRigidBody(body);

			path = "assets\\textures\\terrain\\" + path;
			floorTexture = Util::loadTexture(path.c_str());

			configTerrain();
			configShader();
		}

		void render(Camera& camera, glm::vec3 ambient)
		{
			shader.use();
			glm::mat4 model = glm::mat4(1.0f);
			glm::mat4 view = camera.GetViewMatrix();
			glm::mat4 projection = camera.GetProjectionMatrix();
			shader.setMat4("view", view);
			shader.setMat4("projection", projection);
			shader.setVec3("ambient", ambient);

			glBindVertexArray(VAO);
			glBindTexture(GL_TEXTURE_2D, floorTexture);
			shader.setMat4("model", glm::mat4(1.0f));
			glDrawArrays(GL_TRIANGLES, 0, 6);
			glBindVertexArray(0);

		}

		void render(Avatar& avatar, glm::vec3 ambient)
		{
			shader.use();
			glm::mat4 model = glm::mat4(1.0f);
			glm::mat4 view = avatar.GetViewMatrix();
			glm::mat4 projection = avatar.GetProjectionMatrix();
			shader.setMat4("view", view);
			shader.setMat4("projection", projection);
			shader.setVec3("ambient", ambient);

			glBindVertexArray(VAO);
			glBindTexture(GL_TEXTURE_2D, floorTexture);
			shader.setMat4("model", glm::mat4(1.0f));
			glDrawArrays(GL_TRIANGLES, 0, 6);
			glBindVertexArray(0);
		}

		void configTerrain()
		{
			glDeleteVertexArrays(1, &VAO);
			glDeleteBuffers(1, &VBO);

			planeVertices[0] = volumen.x;
			planeVertices[1] = volumen.y;
			planeVertices[2] = volumen.z;
			planeVertices[3] = repeat;
			planeVertices[4] = 0.0f;

			planeVertices[5] = -volumen.x;
			planeVertices[6] = volumen.y;
			planeVertices[7] = volumen.z;
			planeVertices[8] = 0.0f;
			planeVertices[9] = 0.0f;

			planeVertices[10] = -volumen.x;
			planeVertices[11] = volumen.y;
			planeVertices[12] = -volumen.z;
			planeVertices[13] = 0.0f;
			planeVertices[14] = repeat;

			planeVertices[15] = volumen.x;
			planeVertices[16] = volumen.y;
			planeVertices[17] = volumen.z;
			planeVertices[18] = repeat;
			planeVertices[19] = 0.0f;

			planeVertices[20] = -volumen.x;
			planeVertices[21] = volumen.y;
			planeVertices[22] = -volumen.z;
			planeVertices[23] = 0.0f;
			planeVertices[24] = repeat;

			planeVertices[25] = volumen.x;
			planeVertices[26] = volumen.y;
			planeVertices[27] = -volumen.z;
			planeVertices[28] = repeat;
			planeVertices[29] = repeat;

			glGenVertexArrays(1, &VAO);
			glGenBuffers(1, &VBO);
			glBindVertexArray(VAO);
			glBindBuffer(GL_ARRAY_BUFFER, VBO);
			glBufferData(GL_ARRAY_BUFFER, sizeof(planeVertices), &planeVertices, GL_STATIC_DRAW);
			glEnableVertexAttribArray(0);
			glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
			glEnableVertexAttribArray(1);
			glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
			glBindVertexArray(0);

		}

		void configShader()
		{
			shader.use();
			shader.setInt("textureT", 0);
		}
	};
}

#endif

