#ifndef DYNAMIC_GAME_OBJECT_HEADER
#define DYNAMIC_GAME_OBJECT_HEADER

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include <unordered_map>

#include "MeshA.h"

namespace Cogravi {

	class DynamicGameObject
	{
	public:

		static const GLuint MAX_BONES = 100;
		static const GLuint MAX_ANIMATIONS = 20;
		
		glm::vec3 position;
		glm::vec3 rotation;
		glm::vec3 scale;

		glm::mat4 transform;

		vector<Texture> textures;
		vector<Texture> textures_loaded;
		vector<MeshA> meshes;

		string directory;
		bool textureAssimp;

		Assimp::Importer import[MAX_ANIMATIONS];
		const aiScene* scene[MAX_ANIMATIONS];

		map<string, GLuint> mapBone;
		GLuint numBones = 0;
		vector<BoneMatrix> matrixBone;
		aiMatrix4x4 matrixInverse;

		GLuint boneID;
		float ticksPerSecond = 0.0f;

		std::unordered_map<string, const aiNodeAnim*> mapNodeAnim[MAX_ANIMATIONS];

		vector<const aiAnimation*> animations;
		vector<aiNode*> root;

		GLuint numAnimations = 0;
		GLuint cantidad;
		int currentAnimation;

		//glm::quat rotate_head_xz = glm::quat(cos(glm::radians(0.0f)), sin(glm::radians(0.0f)) * glm::vec3(1.0f, 0.0f, 0.0f));

		DynamicGameObject(glm::vec3 position, glm::vec3 rotation, glm::vec3 scale, string const& path, Shader& shader, vector<Texture> textures = {}, int cantidad = 1)
		{
			this->position = position;
			this->rotation = rotation;
			this->scale = scale;
			this->textures = textures;
			this->textureAssimp = this->textures.size() == 0 ? true : false;
			this->cantidad = cantidad;

			boneID = glGetUniformLocation(shader.ID, "bones[0]");
		
			loadModel(path);		

			if (this->cantidad > 1)
				configureInstance();
		}

		~DynamicGameObject() {}

		void drawInstance(Shader& shader, float animationTime)
		{
			vector<aiMatrix4x4> transforms;
			boneTransform((double)animationTime, transforms);

			glUniformMatrix4fv(boneID, transforms.size(), GL_TRUE, (const GLfloat*)&transforms[0]);

			for (unsigned int i = 0; i < meshes.size(); i++)
				meshes[i].drawInstance(shader, this->cantidad);
		}


		virtual void renderInstance(Camera& camera, int index, Shader& shader, float animationTime)
		{
			glm::mat4 projection = camera.GetProjectionMatrix();
			glm::mat4 view = camera.GetViewMatrix();
			shader.use();
			shader.setMat4("projection", projection);
			shader.setMat4("view", view);

			drawInstance(shader, animationTime);
		}

		void configureInstance()
		{
			glm::mat4* modelMatrices;
			modelMatrices = new glm::mat4[cantidad];
			srand(glfwGetTime()); // initialize random seed	
			float radius = 45.0f;
			float offset = 25.0f;
			for (unsigned int i = 0; i < cantidad; i++)
			{
				glm::mat4 model = glm::mat4(1.0f);
				// 1. translation: displace along circle with 'radius' in range [-offset, offset]
				float angle = (float)i / (float)cantidad * 360.0f;
				float displacement = (rand() % (int)(2 * offset * 100)) / 100.0f - offset;
				float x = sin(angle) * radius + displacement;
				displacement = (rand() % (int)(2 * offset * 100)) / 100.0f - offset;
				float y = displacement * 0.4f; // keep height of asteroid field smaller compared to width of x and z
				displacement = (rand() % (int)(2 * offset * 100)) / 100.0f - offset;
				float z = cos(angle) * radius + displacement;
				model = glm::translate(model, glm::vec3(x, 0, z));

				// 2. scale: Scale between 0.05 and 0.25f
				float scale = (rand() % 20) / 100.0f + 0.05;
				model = glm::scale(model, glm::vec3(0.03f));

				// 3. rotation: add random rotation around a (semi)randomly picked rotation axis vector
				float rotAngle = (rand() % 360);
				model = glm::rotate(model, glm::radians(0.0f), glm::vec3(1.0f, 0.f, 0.f));

				// 4. now add to list of matrices
				modelMatrices[i] = model;
			}

			// configure instanced array
			// -------------------------
			unsigned int buffer;
			glGenBuffers(1, &buffer);
			glBindBuffer(GL_ARRAY_BUFFER, buffer);
			glBufferData(GL_ARRAY_BUFFER, cantidad * sizeof(glm::mat4), &modelMatrices[0], GL_STATIC_DRAW);

			// set transformation matrices as an instance vertex attribute (with divisor 1)
			// note: we're cheating a little by taking the, now publicly declared, VAO of the model's mesh(es) and adding new vertexAttribPointers
			// normally you'd want to do this in a more organized fashion, but for learning purposes this will do.
			// -----------------------------------------------------------------------------------------------------------------------------------
			for (unsigned int i = 0; i < meshes.size(); i++)
			{
				unsigned int VAO = meshes[i].VAO;
				glBindVertexArray(VAO);
				// set attribute pointers for matrix (4 times vec4)
				glEnableVertexAttribArray(5);
				glVertexAttribPointer(5, 4, GL_FLOAT, GL_FALSE, sizeof(glm::mat4), (void*)0);
				glEnableVertexAttribArray(6);
				glVertexAttribPointer(6, 4, GL_FLOAT, GL_FALSE, sizeof(glm::mat4), (void*)(sizeof(glm::vec4)));
				glEnableVertexAttribArray(7);
				glVertexAttribPointer(7, 4, GL_FLOAT, GL_FALSE, sizeof(glm::mat4), (void*)(2 * sizeof(glm::vec4)));
				glEnableVertexAttribArray(8);
				glVertexAttribPointer(8, 4, GL_FLOAT, GL_FALSE, sizeof(glm::mat4), (void*)(3 * sizeof(glm::vec4)));

				glVertexAttribDivisor(5, 1);
				glVertexAttribDivisor(6, 1);
				glVertexAttribDivisor(7, 1);
				glVertexAttribDivisor(8, 1);

				glBindVertexArray(0);
			}

		}

		void draw(Shader &shader, float animationTime)
		{
			vector<aiMatrix4x4> transforms;

			boneTransform((double)animationTime, transforms);
			glUniformMatrix4fv(boneID, transforms.size(), GL_TRUE, (const GLfloat*)&transforms[0]);

			for (unsigned int i = 0; i < meshes.size(); i++)
				meshes[i].draw(shader);
		}

		virtual void render(Camera& camera, Shader &shader, float animationTime)
		{
			shader.use();
			glm::mat4 projection = camera.GetProjectionMatrix();
			glm::mat4 view = camera.GetViewMatrix();

			transform = glm::mat4(1.0f);
			transform = glm::translate(transform, position);

			transform = glm::rotate(transform, glm::radians(rotation.y), glm::vec3(0.0f, 1.0f, 0.0f));
			transform = glm::rotate(transform, glm::radians(rotation.z), glm::vec3(0.0f, 0.0f, 1.0f));
			transform = glm::rotate(transform, glm::radians(rotation.x), glm::vec3(1.0f, 0.0f, 0.0f));

			transform = glm::scale(transform, scale);

			glUniform3f(glGetUniformLocation(shader.ID, "view_pos"), camera.Position.x, camera.Position.y, camera.Position.z);
			glUniform1f(glGetUniformLocation(shader.ID, "material.shininess"), 32.0f);
			glUniform1f(glGetUniformLocation(shader.ID, "material.transparency"), 1.0f);
			// Point Light 1
			glUniform3f(glGetUniformLocation(shader.ID, "point_light.position"), camera.Position.x, camera.Position.y, camera.Position.z);

			glUniform3f(glGetUniformLocation(shader.ID, "point_light.ambient"), 0.1f, 0.1f, 0.1f);
			glUniform3f(glGetUniformLocation(shader.ID, "point_light.diffuse"), 1.0f, 1.0f, 1.0f);
			glUniform3f(glGetUniformLocation(shader.ID, "point_light.specular"), 1.0f, 1.0f, 1.0f);

			glUniform1f(glGetUniformLocation(shader.ID, "point_light.constant"), 1.0f);
			glUniform1f(glGetUniformLocation(shader.ID, "point_light.linear"), 0.007);
			glUniform1f(glGetUniformLocation(shader.ID, "point_light.quadratic"), 0.0002);

			glm::mat4 MVP = projection * view * transform;
			glUniformMatrix4fv(glGetUniformLocation(shader.ID, "MVP"), 1, GL_FALSE, glm::value_ptr(MVP));
			glUniformMatrix4fv(glGetUniformLocation(shader.ID, "M_matrix"), 1, GL_FALSE, glm::value_ptr(transform));
			glm::mat4 matr_normals_cube = glm::mat4(1.0);
			//glm::mat4 matr_normals_cube = glm::mat4(glm::transpose(glm::inverse(transform)));
			glUniformMatrix4fv(glGetUniformLocation(shader.ID, "normals_matrix"), 1, GL_FALSE, glm::value_ptr(matr_normals_cube));

			draw(shader, animationTime);
		}

		virtual void render(Avatar& avatar, Shader& shader, float animationTime)
		{
			shader.use();
			glm::mat4 projection = avatar.proj;
			glm::mat4 view = avatar.view;

			transform = glm::mat4(1.0f);

			transform = glm::rotate(transform, glm::radians(rotation.y), glm::vec3(0.0f, 1.0f, 0.0f));
			transform = glm::rotate(transform, glm::radians(rotation.z), glm::vec3(0.0f, 0.0f, 1.0f));
			transform = glm::rotate(transform, glm::radians(rotation.x), glm::vec3(1.0f, 0.0f, 0.0f));

			transform = glm::scale(transform, scale);

			glm::vec3 positionAvatar = glm::vec3(avatar.position.x, avatar.position.y, avatar.position.z);

			glUniform3f(glGetUniformLocation(shader.ID, "view_pos"), positionAvatar.x, positionAvatar.y, positionAvatar.z);
			glUniform1f(glGetUniformLocation(shader.ID, "material.shininess"), 32.0f);
			glUniform1f(glGetUniformLocation(shader.ID, "material.transparency"), 1.0f);
			// Point Light 1
			glUniform3f(glGetUniformLocation(shader.ID, "point_light.position"), positionAvatar.x, positionAvatar.y, positionAvatar.z);

			glUniform3f(glGetUniformLocation(shader.ID, "point_light.ambient"), 0.1f, 0.1f, 0.1f);
			glUniform3f(glGetUniformLocation(shader.ID, "point_light.diffuse"), 1.0f, 1.0f, 1.0f);
			glUniform3f(glGetUniformLocation(shader.ID, "point_light.specular"), 1.0f, 1.0f, 1.0f);

			glUniform1f(glGetUniformLocation(shader.ID, "point_light.constant"), 1.0f);
			glUniform1f(glGetUniformLocation(shader.ID, "point_light.linear"), 0.007);
			glUniform1f(glGetUniformLocation(shader.ID, "point_light.quadratic"), 0.0002);

			glm::mat4 MVP = projection * view * transform;
			glUniformMatrix4fv(glGetUniformLocation(shader.ID, "MVP"), 1, GL_FALSE, glm::value_ptr(MVP));
			glUniformMatrix4fv(glGetUniformLocation(shader.ID, "M_matrix"), 1, GL_FALSE, glm::value_ptr(transform));
			glm::mat4 matr_normals_cube = glm::mat4(glm::transpose(glm::inverse(transform)));
			glUniformMatrix4fv(glGetUniformLocation(shader.ID, "normals_matrix"), 1, GL_FALSE, glm::value_ptr(matr_normals_cube));


			draw(shader, animationTime);
		}

		virtual void update()
		{

		}

		void addAnimation(string const& path)
		{
			numAnimations++;
			const string ruta = directory + "/" + path;
			scene[numAnimations] = import[numAnimations].ReadFile(ruta,
				aiProcess_JoinIdenticalVertices |
				aiProcess_SortByPType |
				aiProcess_Triangulate |
				aiProcess_GenSmoothNormals |
				aiProcess_FlipUVs |
				aiProcess_LimitBoneWeights);

			if (!scene[numAnimations] || scene[numAnimations]->mFlags == AI_SCENE_FLAGS_INCOMPLETE)
			{
				cout << "error assimp : " << import[numAnimations].GetErrorString() << endl;
				return;
			}
			cout << "==============================================================\n";
			for (GLuint i = 0; i < scene[numAnimations]->mNumAnimations; i++)
			{
				const aiAnimation* animation = scene[numAnimations]->mAnimations[i];
				animations.push_back(animation);
				root.push_back(scene[numAnimations]->mRootNode);
				int index = animations.size() - 1;
				for (GLuint j = 0; j < animation->mNumChannels; j++)
				{

					//cout << scene[v]->mAnimations[i]->mChannels[j]->mNodeName.C_Str() << endl;
					
					const aiNodeAnim* node_anim = animation->mChannels[j];
					mapNodeAnim[index][string(node_anim->mNodeName.data)] = node_anim;
				}
			}
			

			cout << "animaciones: " << animations.size() << "\n";
			
		}

	private:

		void loadModel(string const& path)
		{
			scene[0] = import[0].ReadFile(path, 
				aiProcess_JoinIdenticalVertices |
				aiProcess_SortByPType |
				aiProcess_Triangulate |
				aiProcess_GenSmoothNormals |
				aiProcess_FlipUVs |
				aiProcess_LimitBoneWeights);

			if (!scene[0] || scene[0]->mFlags == AI_SCENE_FLAGS_INCOMPLETE || !scene[0]->mRootNode)
			{
				cout << "error assimp : " << import[0].GetErrorString() << endl;
				return;
			}

			matrixInverse = scene[0]->mRootNode->mTransformation;
			matrixInverse.Inverse();
			cout << "Num de animaciones: " << scene[0]->mNumAnimations << "\n";

			loadAnimations();

			directory = path.substr(0, path.find_last_of('/'));
			cout << "Direc: " << directory << "\n";

			cout << "scene->HasAnimations() 1: " << scene[0]->HasAnimations() << endl;
			cout << "scene->mNumMeshes 1: " << scene[0]->mNumMeshes << endl;
			cout << "scene[0]->mAnimations[0]->mNumChannels 1: " << scene[0]->mAnimations[0]->mNumChannels << endl;
			cout << "scene[0]->mAnimations[0]->mDuration 1: " << scene[0]->mAnimations[0]->mDuration << endl;
			cout << "scene[0]->mAnimations[0]->mTicksPerSecond 1: " << scene[0]->mAnimations[0]->mTicksPerSecond << endl << endl;

			cout << "		name nodes : " << endl;
			showNodeName(scene[0]->mRootNode);
			cout << endl;

			cout << "		name bones : " << endl;
			processNode(scene[0]->mRootNode, scene[0]);

			cout << "		name nodes animation : " << endl;
			for (GLuint i = 0; i < scene[0]->mAnimations[0]->mNumChannels; i++)
			{
				cout << scene[0]->mAnimations[0]->mChannels[i]->mNodeName.C_Str() << endl;
			}
			cout << endl;
			glBindVertexArray(0);
		}

		void loadAnimations()
		{
			for (GLuint i = 0; i < scene[0]->mNumAnimations; i++)
			{
				numAnimations++;
				const aiAnimation* animation = scene[0]->mAnimations[i];
				animations.push_back(animation);
				int index = animations.size() - 1;
				root.push_back(scene[0]->mRootNode);
				for (GLuint j = 0; j < animation->mNumChannels; j++)
				{
					const aiNodeAnim* node_anim = animation->mChannels[j];
					mapNodeAnim[index][string(node_anim->mNodeName.data)] = node_anim;
				}
			}
			cout << "animaciones: " << animations.size() << "\n";
		}

		void showNodeName(aiNode* node)
		{
			cout << node->mName.data << endl;
			for (GLuint i = 0; i < node->mNumChildren; i++)
			{
				showNodeName(node->mChildren[i]);
			}
		}

		void processNode(aiNode* node, const aiScene* scene)
		{
			MeshA mesh;
			for (GLuint i = 0; i < scene->mNumMeshes; i++)
			{
				aiMesh* ai_mesh = scene->mMeshes[i];
				mesh = processMesh(ai_mesh, scene);
				meshes.push_back(mesh); //accumulate all meshes in one vector
			}

		}

		MeshA processMesh(aiMesh* mesh, const aiScene* scene)
		{
			// data to fill
			vector<VertexA> vertices;
			vector<GLuint> indices;
			vector<Texture> textures;
			vector<VertexBoneData> bones_id_weights_for_each_vertex;

			vertices.reserve(mesh->mNumVertices);
			indices.reserve(mesh->mNumVertices);

			//first to load all the bones
			//this will do the bone mapping and fill the Bones vector
			bones_id_weights_for_each_vertex.resize(mesh->mNumVertices);

			// Walk through each of the mesh's vertices
			for (GLuint i = 0; i < mesh->mNumVertices; i++)
			{
				VertexA vertex;
				glm::vec3 vector;
				vector.x = mesh->mVertices[i].x;
				vector.y = mesh->mVertices[i].y;
				vector.z = mesh->mVertices[i].z;
				vertex.position = vector;

				if (mesh->mNormals != NULL)
				{
					vector.x = mesh->mNormals[i].x;
					vector.y = mesh->mNormals[i].y;
					vector.z = mesh->mNormals[i].z;
					vertex.normal = vector;
				}
				else
				{
					vertex.normal = glm::vec3(1.0f);
				}


				// in assimp model can have 8 different texture coordinates
				// we only care about the first set of texture coordinates
				if (mesh->mTextureCoords[0])
				{
					glm::vec2 vec;
					vec.x = mesh->mTextureCoords[0][i].x;
					vec.y = mesh->mTextureCoords[0][i].y;
					vertex.text_coords = vec;
				}
				else
				{
					vertex.text_coords = glm::vec2(0.0f, 0.0f);
				}
				vertices.push_back(vertex);
			}

			//indices
			for (GLuint i = 0; i < mesh->mNumFaces; i++)
			{
				aiFace face = mesh->mFaces[i];
				indices.push_back(face.mIndices[0]);
				indices.push_back(face.mIndices[1]);
				indices.push_back(face.mIndices[2]);
			}

			// load bones
			for (GLuint i = 0; i < mesh->mNumBones; i++)
			{
				GLuint bone_index = 0;
				string bone_name(mesh->mBones[i]->mName.data);

				cout << mesh->mBones[i]->mName.data << endl;

				if (mapBone.find(bone_name) == mapBone.end())
				{
					// Allocate an index for a new bone
					bone_index = numBones;
					numBones++;
					BoneMatrix bi;
					matrixBone.push_back(bi);
					matrixBone[bone_index].offset_matrix = mesh->mBones[i]->mOffsetMatrix;
					mapBone[bone_name] = bone_index;

					//cout << "bone_name: " << bone_name << "			 bone_index: " << bone_index << endl;
				}
				else
				{
					bone_index = mapBone[bone_name];
				}

				for (GLuint j = 0; j < mesh->mBones[i]->mNumWeights; j++)
				{
					GLuint vertex_id = mesh->mBones[i]->mWeights[j].mVertexId;
					float weight = mesh->mBones[i]->mWeights[j].mWeight;
					bones_id_weights_for_each_vertex[vertex_id].addBoneData(bone_index, weight);
				}
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
				return MeshA(vertices, indices, textures, bones_id_weights_for_each_vertex);
			}
			else
			{
				return MeshA(vertices, indices, this->textures, bones_id_weights_for_each_vertex);
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

		GLuint findPosition(float p_animation_time, const aiNodeAnim* p_node_anim)
		{
			//cout << "NumPositionKey: " << p_node_anim->mNumPositionKeys << "\n";
			for (GLuint i = 0; i < p_node_anim->mNumPositionKeys - 1; i++)
			{
				if (p_animation_time < (float)p_node_anim->mPositionKeys[i + 1].mTime)
				{
					return i;
				}
			}

			assert(0);
			return 0;
		}

		GLuint findRotation(float p_animation_time, const aiNodeAnim* p_node_anim)
		{
			assert(p_node_anim->mNumRotationKeys > 0);
			for (GLuint i = 0; i < p_node_anim->mNumRotationKeys - 1; i++)
			{
				if (p_animation_time < (float)p_node_anim->mRotationKeys[i + 1].mTime)
				{
					return i;
				}
			}
			assert(0);
			return 0;
		}

		GLuint findScaling(float p_animation_time, const aiNodeAnim* p_node_anim)
		{

			for (GLuint i = 0; i < p_node_anim->mNumScalingKeys - 1; i++)
			{
				if (p_animation_time < (float)p_node_anim->mScalingKeys[i + 1].mTime)
				{
					return i;
				}
			}

			assert(0);
			return 0;
		}

		aiVector3D calcInterpolatedPosition(float p_animation_time, const aiNodeAnim* p_node_anim)
		{
			if (p_node_anim->mNumPositionKeys == 1)
			{
				return p_node_anim->mPositionKeys[0].mValue;
			}

			GLuint position_index = findPosition(p_animation_time, p_node_anim);
			GLuint next_position_index = position_index + 1;
			assert(next_position_index < p_node_anim->mNumPositionKeys);

			float delta_time = (float)(p_node_anim->mPositionKeys[next_position_index].mTime - p_node_anim->mPositionKeys[position_index].mTime);

			float factor = (p_animation_time - (float)p_node_anim->mPositionKeys[position_index].mTime) / delta_time;
			assert(factor >= 0.0f && factor <= 1.0f);
			aiVector3D start = p_node_anim->mPositionKeys[position_index].mValue;
			aiVector3D end = p_node_anim->mPositionKeys[next_position_index].mValue;
			aiVector3D delta = end - start;

			return start + factor * delta;
		}

		aiQuaternion calcInterpolatedRotation(float p_animation_time, const aiNodeAnim* p_node_anim)
		{
			if (p_node_anim->mNumRotationKeys == 1)
			{
				return p_node_anim->mRotationKeys[0].mValue;
			}

			GLuint rotation_index = findRotation(p_animation_time, p_node_anim);
			GLuint next_rotation_index = rotation_index + 1;
			assert(next_rotation_index < p_node_anim->mNumRotationKeys);

			float delta_time = (float)(p_node_anim->mRotationKeys[next_rotation_index].mTime - p_node_anim->mRotationKeys[rotation_index].mTime);

			float factor = (p_animation_time - (float)p_node_anim->mRotationKeys[rotation_index].mTime) / delta_time;

			//cout << "p_node_anim->mRotationKeys[rotation_index].mTime: " << p_node_anim->mRotationKeys[rotation_index].mTime << endl;
			//cout << "p_node_anim->mRotationKeys[next_rotaion_index].mTime: " << p_node_anim->mRotationKeys[next_rotation_index].mTime << endl;
			//cout << "delta_time: " << delta_time << endl;
			//cout << "animation_time: " << p_animation_time << endl;
			//cout << "animation_time - mRotationKeys[rotation_index].mTime: " << (p_animation_time - (float)p_node_anim->mRotationKeys[rotation_index].mTime) << endl;
			//cout << "factor: " << factor << endl << endl << endl;

			assert(factor >= 0.0f && factor <= 1.0f);
			aiQuaternion start_quat = p_node_anim->mRotationKeys[rotation_index].mValue;
			aiQuaternion end_quat = p_node_anim->mRotationKeys[next_rotation_index].mValue;

			/*	aiQuaternion Out;

				aiQuaternion::Interpolate(Out, start_quat, end_quat, factor);

				return Out.Normalize();*/

			return nlerp(start_quat, end_quat, factor);
		}

		aiVector3D calcInterpolatedScaling(float p_animation_time, const aiNodeAnim* p_node_anim)
		{
			if (p_node_anim->mNumScalingKeys == 1)
			{
				return p_node_anim->mScalingKeys[0].mValue;
			}

			GLuint scaling_index = findScaling(p_animation_time, p_node_anim);
			GLuint next_scaling_index = scaling_index + 1;
			assert(next_scaling_index < p_node_anim->mNumScalingKeys);

			float delta_time = (float)(p_node_anim->mScalingKeys[next_scaling_index].mTime - p_node_anim->mScalingKeys[scaling_index].mTime);

			float  factor = (p_animation_time - (float)p_node_anim->mScalingKeys[scaling_index].mTime) / delta_time;
			assert(factor >= 0.0f && factor <= 1.0f);
			aiVector3D start = p_node_anim->mScalingKeys[scaling_index].mValue;
			aiVector3D end = p_node_anim->mScalingKeys[next_scaling_index].mValue;
			aiVector3D delta = end - start;

			return start + factor * delta;
		}


		void readNodeHierarchy(float p_animation_time, const aiNode* p_node, const aiMatrix4x4 parent_transform)
		{

			string node_name(p_node->mName.data);
			const aiAnimation* animation = animations[currentAnimation];
			aiMatrix4x4 node_transform = p_node->mTransformation;
			const aiNodeAnim* node_anim = mapNodeAnim[currentAnimation][node_name];

			if (node_anim)
			{

				//scaling
				//aiVector3D scaling_vector = node_anim->mScalingKeys[2].mValue;
				aiVector3D scaling_vector = calcInterpolatedScaling(p_animation_time, node_anim);
				aiMatrix4x4 scaling_matr;
				aiMatrix4x4::Scaling(scaling_vector, scaling_matr);

				//rotation
				//aiQuaternion rotate_quat = node_anim->mRotationKeys[2].mValue;
				aiQuaternion rotate_quat = calcInterpolatedRotation(p_animation_time, node_anim);
				aiMatrix4x4 rotate_matr = aiMatrix4x4(rotate_quat.GetMatrix());

				//translation
				//aiVector3D translate_vector = node_anim->mPositionKeys[2].mValue;
				aiVector3D translate_vector = calcInterpolatedPosition(p_animation_time, node_anim);
				aiMatrix4x4 translate_matr;
				aiMatrix4x4::Translation(translate_vector, translate_matr);

				/*if (string(node_anim->mNodeName.data) == string("Head"))
				{
					aiQuaternion rotate_head = aiQuaternion(rotate_head_xz.w, rotate_head_xz.x, rotate_head_xz.y, rotate_head_xz.z);

					node_transform = translate_matr * (rotate_matr * aiMatrix4x4(rotate_head.GetMatrix())) * scaling_matr;
				}
				else
				{*/
					node_transform = translate_matr * rotate_matr * scaling_matr;
				//}

			}

			aiMatrix4x4 global_transform = parent_transform * node_transform;


			if (mapBone.find(node_name) != mapBone.end())
			{
				GLuint bone_index = mapBone[node_name];
				matrixBone[bone_index].final_world_transform = matrixInverse * global_transform * matrixBone[bone_index].offset_matrix;
			}

			for (GLuint i = 0; i < p_node->mNumChildren; i++)
			{
				readNodeHierarchy(p_animation_time, p_node->mChildren[i], global_transform);
			}

		}

		void boneTransform(double time_in_sec, vector<aiMatrix4x4>& transforms)
		{
			aiMatrix4x4 identity_matrix; 

			const aiAnimation* animation = animations[currentAnimation];

			if (animation->mTicksPerSecond != 0.0)
				ticksPerSecond = animation->mTicksPerSecond;			
			else
				ticksPerSecond = 25.0f;
		
			double time_in_ticks = time_in_sec * ticksPerSecond;
			float animation_time = fmod(time_in_ticks, animation->mDuration);

			//cout << "Ti: " << animation_time << "\n";

			readNodeHierarchy(animation_time, root[currentAnimation], identity_matrix);

			transforms.resize(numBones);

			for (GLuint i = 0; i < numBones; i++)
			{
				transforms[i] = matrixBone[i].final_world_transform;
			}
		}

		glm::mat4 aiToGlm(aiMatrix4x4 ai_matr)
		{
			glm::mat4 result;
			result[0].x = ai_matr.a1; result[0].y = ai_matr.b1; result[0].z = ai_matr.c1; result[0].w = ai_matr.d1;
			result[1].x = ai_matr.a2; result[1].y = ai_matr.b2; result[1].z = ai_matr.c2; result[1].w = ai_matr.d2;
			result[2].x = ai_matr.a3; result[2].y = ai_matr.b3; result[2].z = ai_matr.c3; result[2].w = ai_matr.d3;
			result[3].x = ai_matr.a4; result[3].y = ai_matr.b4; result[3].z = ai_matr.c4; result[3].w = ai_matr.d4;

			return result;
		}

		aiQuaternion nlerp(aiQuaternion a, aiQuaternion b, float blend)
		{
			//cout << a.w + a.x + a.y + a.z << endl;
			a.Normalize();
			b.Normalize();

			aiQuaternion result;
			float dot_product = a.x * b.x + a.y * b.y + a.z * b.z + a.w * b.w;
			float one_minus_blend = 1.0f - blend;

			if (dot_product < 0.0f)
			{
				result.x = a.x * one_minus_blend + blend * -b.x;
				result.y = a.y * one_minus_blend + blend * -b.y;
				result.z = a.z * one_minus_blend + blend * -b.z;
				result.w = a.w * one_minus_blend + blend * -b.w;
			}
			else
			{
				result.x = a.x * one_minus_blend + blend * b.x;
				result.y = a.y * one_minus_blend + blend * b.y;
				result.z = a.z * one_minus_blend + blend * b.z;
				result.w = a.w * one_minus_blend + blend * b.w;
			}

			return result.Normalize();
		}

	};
}
#endif
