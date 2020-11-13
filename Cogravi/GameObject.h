#ifndef GAME_OBJECT_HEADER
#define GAME_OBJECT_HEADER

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h> 
#include "MeshM.h"

namespace Cogravi {

    class GameObject
    {
    public:

        glm::vec3 position;
        glm::vec3 rotation;
        glm::vec3 scale;

		glm::mat4 transform;

        vector<Texture> textures;
        vector<Texture> textures_loaded;
        vector<MeshM> meshes;

		string directory;
		bool textureAssimp;

		unsigned int cantidad;


		GameObject(glm::vec3 position, glm::vec3 rotation, glm::vec3 scale, string const& path, vector<Texture>textures = {}, int cantidad = 1)
		{
			this->position = position;
			this->rotation = rotation;
			this->scale = scale;		
			this->textures = textures;
			this->textureAssimp = this->textures.size() == 0 ? true : false;
			this->cantidad = cantidad;

			if (this->cantidad > 1)
				configureInstance();

			loadModel(path);
		}

		~GameObject() {}

		void draw(Shader &shader)
		{
			for (unsigned int i = 0; i < meshes.size(); i++)
				meshes[i].draw(shader);
		}

		void drawFramebuffer(Shader& shader, int index)
		{
			for (unsigned int i = 0; i < meshes.size(); i++)
			{
				if (index == i)
				{
					shader.setBool("isTextureFrambuffer", true);
					meshes[i].draw(shader);
					shader.setBool("isTextureFrambuffer", false);
				}
				else
				{
					meshes[i].draw(shader);
				}
			}
		}

		void drawInstance(Shader &shader)
		{
			for (unsigned int i = 0; i < meshes.size(); i++)
				meshes[i].drawInstance(shader, this->cantidad);
		}	

		virtual void renderInstance(Camera& camera, Shader& shader)
		{
			glm::mat4 projection = camera.GetProjectionMatrix();
			glm::mat4 view = camera.GetViewMatrix();
			shader.use();

			shader.setVec3("viewPos", camera.Position);
			shader.setMat4("projection", projection);
			shader.setMat4("view", view);
			
			drawInstance(shader);
		}

		virtual void renderInstance(Avatar& avatar, Shader& shader)
		{
			glm::mat4 projection = avatar.GetProjectionMatrix();
			glm::mat4 view = avatar.GetViewMatrix();
			shader.use();

			shader.setVec3("viewPos", _glmFromOvrVector(avatar.position));
			shader.setMat4("projection", projection);
			shader.setMat4("view", view);

			drawInstance(shader);
		}

		void configureInstance()
		{		
			glm::mat4* modelMatrices;
			modelMatrices = new glm::mat4[cantidad];
			srand(time(NULL)); 
			float radius = 65.0f;
			float offset = 25.0f;
			for (unsigned int i = 0; i < cantidad; i++)
			{
				glm::mat4 model = glm::mat4(1.0f);
				
				float angle = (float)i / (float)cantidad * 360.0f;
				float displacement = (rand() % (int)(2 * offset * 100)) / 100.0f - offset;
				float x = sin(angle) * radius + displacement;
				displacement = (rand() % (int)(2 * offset * 100)) / 100.0f - offset;
				float y = 0.0f;
				displacement = (rand() % (int)(2 * offset * 100)) / 100.0f - offset;
				float z = cos(angle) * radius + displacement;
				model = glm::translate(model, glm::vec3(x, y, z));

				float scale = (rand() % 20) / 100.0f + 0.05;
				model = glm::scale(model, glm::vec3(scale*2));

				float rotAngle = (rand() % 360);
				//model = glm::rotate(model, rotAngle, glm::vec3(0.4f, 0.6f, 0.8f));

				modelMatrices[i] = model;
			}


			unsigned int buffer;
			glGenBuffers(1, &buffer);
			glBindBuffer(GL_ARRAY_BUFFER, buffer);
			glBufferData(GL_ARRAY_BUFFER, cantidad * sizeof(glm::mat4), &modelMatrices[0], GL_STATIC_DRAW);
			
			for (unsigned int i = 0; i < meshes.size(); i++)
			{
				unsigned int VAO = meshes[i].VAO;
				glBindVertexArray(VAO);

				glEnableVertexAttribArray(3);
				glVertexAttribPointer(3, 4, GL_FLOAT, GL_FALSE, sizeof(glm::mat4), (void*)0);
				glEnableVertexAttribArray(4);
				glVertexAttribPointer(4, 4, GL_FLOAT, GL_FALSE, sizeof(glm::mat4), (void*)(sizeof(glm::vec4)));
				glEnableVertexAttribArray(5);
				glVertexAttribPointer(5, 4, GL_FLOAT, GL_FALSE, sizeof(glm::mat4), (void*)(2 * sizeof(glm::vec4)));
				glEnableVertexAttribArray(6);
				glVertexAttribPointer(6, 4, GL_FLOAT, GL_FALSE, sizeof(glm::mat4), (void*)(3 * sizeof(glm::vec4)));

				glVertexAttribDivisor(3, 1);
				glVertexAttribDivisor(4, 1);
				glVertexAttribDivisor(5, 1);
				glVertexAttribDivisor(6, 1);

				glBindVertexArray(0);
			}

		}

		virtual void render(Camera& camera, Shader &shader)
		{
			shader.use();

			glm::mat4 view = camera.GetViewMatrix();
			glm::mat4 projection = camera.GetProjectionMatrix();

			transform = glm::mat4(1.0f);

			transform = glm::translate(transform, position);

			transform = glm::rotate(transform, glm::radians(rotation.y), glm::vec3(0.0f, 1.0f, 0.0f));
			transform = glm::rotate(transform, glm::radians(rotation.z), glm::vec3(0.0f, 0.0f, 1.0f));
			transform = glm::rotate(transform, glm::radians(rotation.x), glm::vec3(1.0f, 0.0f, 0.0f));

			transform = glm::scale(transform, scale);

			shader.setVec3("viewPos", camera.Position);

			shader.setMat4("model", transform);
			shader.setMat4("view", view);
			shader.setMat4("projection", projection);

			draw(shader);
		}

		virtual void render(Avatar& avatar, Shader& shader)
		{
			shader.use();

			glm::mat4 projection = avatar.GetProjectionMatrix();
			glm::mat4 view = avatar.GetViewMatrix();

			transform = glm::mat4(1.0f);

			transform = glm::translate(transform, position);

			transform = glm::rotate(transform, glm::radians(rotation.y), glm::vec3(0.0f, 1.0f, 0.0f));
			transform = glm::rotate(transform, glm::radians(rotation.z), glm::vec3(0.0f, 0.0f, 1.0f));
			transform = glm::rotate(transform, glm::radians(rotation.x), glm::vec3(1.0f, 0.0f, 0.0f));

			transform = glm::scale(transform, scale);

			glm::vec3 posAvatar = glm::vec3(avatar.position.x, avatar.position.y, avatar.position.z);
			shader.setVec3("viewPos", posAvatar);

			shader.setMat4("model", transform);
			shader.setMat4("view", view);
			shader.setMat4("projection", projection);

			draw(shader);
		}

		

		virtual void update()
		{

		}

		void loadModel(string const& path)
		{
			Assimp::Importer importer;
			const aiScene* scene = importer.ReadFile(path, aiProcess_Triangulate | aiProcess_FlipUVs | aiProcess_CalcTangentSpace);
			if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode)
			{
				cout << "ERROR::ASSIMP:: " << importer.GetErrorString() << endl;
				return;
			}
			directory = path.substr(0, path.find_last_of('/'));

			processNode(scene->mRootNode, scene);
		}

		void processNode(aiNode* node, const aiScene* scene)
		{
			for (unsigned int i = 0; i < node->mNumMeshes; i++)
			{
				aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
				meshes.push_back(processMesh(mesh, scene));
			}
			for (unsigned int i = 0; i < node->mNumChildren; i++)
			{
				processNode(node->mChildren[i], scene);
			}
		}

		MeshM processMesh(aiMesh* mesh, const aiScene* scene)
		{
			vector<Vertex> vertices;
			vector<unsigned int> indices;
			vector<Texture> textures;

			for (unsigned int i = 0; i < mesh->mNumVertices; i++)
			{
				Vertex vertex;
				glm::vec3 vector;
				vector.x = mesh->mVertices[i].x;
				vector.y = mesh->mVertices[i].y;
				vector.z = mesh->mVertices[i].z;
				vertex.Position = vector;

				if (mesh->HasNormals())
				{
					vector.x = mesh->mNormals[i].x;
					vector.y = mesh->mNormals[i].y;
					vector.z = mesh->mNormals[i].z;
					vertex.Normal = vector;
				}

				if (mesh->mTextureCoords[0])
				{
					glm::vec2 vec;
					vec.x = mesh->mTextureCoords[0][i].x;
					vec.y = mesh->mTextureCoords[0][i].y;
					vertex.TexCoords = vec;
				}
				else
				{
					vertex.TexCoords = glm::vec2(0.0f, 0.0f);
				}

				if (mesh->HasTangentsAndBitangents())
				{
					vector.x = mesh->mTangents[i].x;
					vector.y = mesh->mTangents[i].y;
					vector.z = mesh->mTangents[i].z;
					vertex.Tangent = vector;

					vector.x = mesh->mBitangents[i].x;
					vector.y = mesh->mBitangents[i].y;
					vector.z = mesh->mBitangents[i].z;
					vertex.Bitangent = vector;
				}
				
				vertices.push_back(vertex);
			}

			for (unsigned int i = 0; i < mesh->mNumFaces; i++)
			{
				aiFace face = mesh->mFaces[i];

				for (unsigned int j = 0; j < face.mNumIndices; j++)
					indices.push_back(face.mIndices[j]);
			}


			if (textureAssimp)
			{
				aiMaterial* material = scene->mMaterials[mesh->mMaterialIndex];
				vector<Texture> diffuseMaps = loadMaterialTextures(material, aiTextureType_DIFFUSE, TextureType::DIFFUSE);
				textures.insert(textures.end(), diffuseMaps.begin(), diffuseMaps.end());

				vector<Texture> specularMaps = loadMaterialTextures(material, aiTextureType_SPECULAR, TextureType::SPECULAR);
				textures.insert(textures.end(), specularMaps.begin(), specularMaps.end());

				std::vector<Texture> normalMaps = loadMaterialTextures(material, aiTextureType_HEIGHT, TextureType::NORMAL);
				textures.insert(textures.end(), normalMaps.begin(), normalMaps.end());

				//std::vector<Texture> heightMaps = loadMaterialTextures(material, aiTextureType_AMBIENT, "texture_height");
				//textures.insert(textures.end(), heightMaps.begin(), heightMaps.end());
				return MeshM(vertices, indices, textures);
			}
			else
			{
				return MeshM(vertices, indices, this->textures);
			}

		}

		vector<Texture> loadMaterialTextures(aiMaterial* mat, aiTextureType type, TextureType typeName)
		{
			vector<Texture> textures;
			for (unsigned int i = 0; i < mat->GetTextureCount(type); i++)
			{
				aiString str;
				mat->GetTexture(type, i, &str);

				bool skip = false;
				for (unsigned int j = 0; j < textures_loaded.size(); j++)
				{
					if (std::strcmp(textures_loaded[j].path.data(), str.C_Str()) == 0)
					{
						textures.push_back(textures_loaded[j]);
						skip = true;
						break;
					}
				}
				if (!skip)
				{
					Texture texture;
					texture.id = Util::TextureFromFile(str.C_Str(), this->directory);
					texture.type = typeName;
					texture.path = str.C_Str();
					textures.push_back(texture);
					textures_loaded.push_back(texture);
				}
			}
			return textures;
		}

    };
}
#endif