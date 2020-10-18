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

		int slices = 10;
		int lenght;

		Terrain(string model, glm::vec3 volumen, float repeat, Shader shader, BulletWorldController* bulletWorldController)
		{
			this->shader = shader;
			this->volumen = volumen;
			this->repeat = repeat;

			btTransform transform;
			transform.setIdentity();
			transform.setOrigin(btVector3(0, 0, 0));

			//Configuramos las propiedades básicas de construcción del cuerpo

			plane = new btStaticPlaneShape(btVector3(0, 1, 0), 0);
			btMotionState* motion = new btDefaultMotionState(transform);
			btRigidBody::btRigidBodyConstructionInfo info(0.0, motion, plane);

			//Establecemos los parámetros que recibidos como parámetro
			btRigidBody* body = new btRigidBody(info);
			//body = new btRigidBody(info);

			//Añadimos el cuerpo al mundo dinámico
			bulletWorldController->dynamicsWorld->addRigidBody(body);

			//Guardamos la información del modelo en vectores de punteros
			//bulletWorldController->rigidBodies.push_back(body);

			floorTexture = Util::loadTexture(model.c_str());

			configTerrain();
			configShader();
			//grid();
		}

		void render(Camera& camera, glm::vec3 ambient)
		{
			shader.use();
			glm::mat4 model = glm::mat4(1.0f);
			glm::mat4 view = camera.GetViewMatrix();
			glm::mat4 projection = glm::perspective(glm::radians(camera.FOV), (float)WIDTH / (float)HEIGHT, camera.NEAR, camera.FAR);
			shader.setMat4("view", view);
			shader.setMat4("projection", projection);
			shader.setVec3("ambient", ambient);

			glBindVertexArray(VAO);
			glBindTexture(GL_TEXTURE_2D, floorTexture);
			shader.setMat4("model", glm::mat4(1.0f));
			glDrawArrays(GL_TRIANGLES, 0, 6);
			glBindVertexArray(0);

		}

		void render(Avatar& avatar)
		{
			shader.use();
			glm::mat4 model = glm::mat4(1.0f);
			glm::mat4 view = avatar.view;
			glm::mat4 projection = avatar.proj;
			shader.setMat4("view", view);
			shader.setMat4("projection", projection);

			glBindVertexArray(VAO);
			glBindTexture(GL_TEXTURE_2D, floorTexture);
			shader.setMat4("model", glm::mat4(1.0f));
			glDrawArrays(GL_TRIANGLES, 0, 6);
			glBindVertexArray(0);
		}
		

		void grid()
		{
			std::vector<glm::vec3> vertices;
			std::vector<glm::uvec4> indices;

			for (int j = 0; j <= slices; ++j) {
				for (int i = 0; i <= slices; ++i) {
					float x = (float)i / (float)slices;
					float y = 0;
					float z = (float)j / (float)slices;
					vertices.push_back(glm::vec3(x, y, z));
				}
			}

			for (int j = 0; j < slices; ++j) {
				for (int i = 0; i < slices; ++i) {

					int row1 = j * (slices + 1);
					int row2 = (j + 1) * (slices + 1);

					indices.push_back(glm::uvec4(row1 + i, row1 + i + 1, row1 + i + 1, row2 + i + 1));
					indices.push_back(glm::uvec4(row2 + i + 1, row2 + i, row2 + i, row1 + i));

				}
			}

			glGenVertexArrays(1, &VAO);
			glBindVertexArray(VAO);

			GLuint vbo;
			glGenBuffers(1, &vbo);
			glBindBuffer(GL_ARRAY_BUFFER, vbo);
			glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(glm::vec3), glm::value_ptr(vertices[0]), GL_STATIC_DRAW);
			glEnableVertexAttribArray(0);
			glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, nullptr);

			GLuint ibo;
			glGenBuffers(1, &ibo);
			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);
			glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(glm::uvec4), glm::value_ptr(indices[0]), GL_STATIC_DRAW);

			glBindVertexArray(0);
			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
			glBindBuffer(GL_ARRAY_BUFFER, 0);

			lenght = (GLuint)indices.size() * 4;
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
			shader.setInt("texture1", 0);
		}
	};
}

#endif

