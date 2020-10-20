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


		GameObject(glm::vec3 position, glm::vec3 rotation, glm::vec3 scale, string const& path, vector<Texture>textures = {})
		{
			this->position = position;
			this->rotation = rotation;
			this->scale = scale;
		
			this->textures = textures;

			this->textureAssimp = this->textures.size() == 0 ? true : false;

			loadModel(path);
		}

		~GameObject() {}

		void draw(Shader shader)
		{
			for (unsigned int i = 0; i < meshes.size(); i++)
				meshes[i].draw(shader);
		}

		virtual void render(Camera& camera, Shader &shader)
		{
			shader.use();

			glm::mat4 view = camera.GetViewMatrix();
			glm::mat4 projection = glm::perspective(glm::radians(camera.FOV), (float)WIDTH / (float)HEIGHT, camera.NEAR, camera.FAR);

			transform = glm::mat4(1.0f);

			transform = glm::translate(transform, position);

			transform = glm::rotate(transform, glm::radians(rotation.y), glm::vec3(0.0f, 1.0f, 0.0f));
			transform = glm::rotate(transform, glm::radians(rotation.z), glm::vec3(0.0f, 0.0f, 1.0f));
			transform = glm::rotate(transform, glm::radians(rotation.x), glm::vec3(1.0f, 0.0f, 0.0f));

			transform = glm::scale(transform, scale);

			shader.setVec3("lightPos", glm::vec3(2, 4, 0));
			shader.setVec3("viewPos", camera.Position);

			shader.setMat4("model", transform);
			shader.setMat4("view", view);
			shader.setMat4("projection", projection);

			draw(shader);
		}

		virtual void render(Avatar& avatar, Shader& shader)
		{
			shader.use();

			glm::mat4 projection = avatar.proj;
			glm::mat4 view = avatar.view;

			transform = glm::mat4(1.0f);

			transform = glm::translate(transform, position);

			transform = glm::rotate(transform, glm::radians(rotation.y), glm::vec3(0.0f, 1.0f, 0.0f));
			transform = glm::rotate(transform, glm::radians(rotation.z), glm::vec3(0.0f, 0.0f, 1.0f));
			transform = glm::rotate(transform, glm::radians(rotation.x), glm::vec3(1.0f, 0.0f, 0.0f));

			transform = glm::scale(transform, scale);

			shader.setVec3("lightPos", glm::vec3(2, 4, 0));
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

				vector.x = mesh->mNormals[i].x;
				vector.y = mesh->mNormals[i].y;
				vector.z = mesh->mNormals[i].z;
				vertex.Normal = vector;
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

				vector.x = mesh->mTangents[i].x;
				vector.y = mesh->mTangents[i].y;
				vector.z = mesh->mTangents[i].z;
				vertex.Tangent = vector;

				vector.x = mesh->mBitangents[i].x;
				vector.y = mesh->mBitangents[i].y;
				vector.z = mesh->mBitangents[i].z;
				vertex.Bitangent = vector;
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