#ifndef DYNAMIC_GAME_OBJECT_HEADER
#define DYNAMIC_GAME_OBJECT_HEADER

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h> 

#include "MeshA.h"

namespace Cogravi {

	class DynamicGameObject
	{
	public:

		static const GLuint MAX_BONES = 100;

		glm::vec3 position;
		glm::vec3 rotation;
		glm::vec3 scale;

		glm::mat4 transform;

		vector<Texture> textures;
		vector<Texture> textures_loaded;
		vector<MeshA> meshes;

		string directory;
		bool textureAssimp;

		//GLint  indexAmount;           //Tamaño del vector de indices del modelo

		Assimp::Importer import;
		const aiScene* scene;

		map<string, GLuint> m_bone_mapping; // maps a bone name and their index
		GLuint m_num_bones = 0;
		vector<BoneMatrix> m_bone_matrices;
		aiMatrix4x4 m_global_inverse_transform;

		GLuint m_bone_location[MAX_BONES];
		float ticks_per_second = 0.0f;

		glm::quat rotate_head_xz = glm::quat(cos(glm::radians(0.0f)), sin(glm::radians(0.0f)) * glm::vec3(1.0f, 0.0f, 0.0f));

		DynamicGameObject(glm::vec3 position, glm::vec3 rotation, glm::vec3 scale, string const& path, Shader& shader, vector<Texture> textures = {})
		{
			this->position = position;
			this->rotation = rotation;
			this->scale = scale;
			this->textures = textures;

			this->textureAssimp = this->textures.size() == 0 ? true : false;

			for (GLuint i = 0; i < MAX_BONES; i++)
			{
				string name = "bones[" + to_string(i) + "]";
				m_bone_location[i] = glGetUniformLocation(shader.ID, name.c_str());
			}

			loadModel(path);
		}

		~DynamicGameObject() {}

		void draw(Shader shader, float animationTime)
		{
			vector<aiMatrix4x4> transforms;
			boneTransform((double)animationTime, transforms);

			for (GLuint i = 0; i < transforms.size(); i++)
			{
				glUniformMatrix4fv(m_bone_location[i], 1, GL_TRUE, (const GLfloat*)&transforms[i]);
			}

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

	private:

		void loadModel(string const& path)
		{
			scene = import.ReadFile(path, 
				aiProcess_JoinIdenticalVertices |
				aiProcess_SortByPType |
				aiProcess_Triangulate |
				aiProcess_GenSmoothNormals |
				aiProcess_FlipUVs |
				aiProcess_LimitBoneWeights);

			if (!scene || scene->mFlags == AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode)
			{
				cout << "error assimp : " << import.GetErrorString() << endl;
				return;
			}

			m_global_inverse_transform = scene->mRootNode->mTransformation;
			m_global_inverse_transform.Inverse();
			cout << "Num de animaciones: " << scene->mNumAnimations << "\n";

			if (scene->mNumAnimations > 0)
			{

				if (scene->mAnimations[0]->mTicksPerSecond != 0.0)
				{
					ticks_per_second = scene->mAnimations[0]->mTicksPerSecond;
				}
				else
				{
					ticks_per_second = 25.0f;
				}
			}

			// directoru = container for model.obj and textures and other files
			directory = path.substr(0, path.find_last_of('/'));

			cout << "scene->HasAnimations() 1: " << scene->HasAnimations() << endl;
			cout << "scene->mNumMeshes 1: " << scene->mNumMeshes << endl;
			cout << "scene->mAnimations[0]->mNumChannels 1: " << scene->mAnimations[0]->mNumChannels << endl;
			cout << "scene->mAnimations[0]->mDuration 1: " << scene->mAnimations[0]->mDuration << endl;
			cout << "scene->mAnimations[0]->mTicksPerSecond 1: " << scene->mAnimations[0]->mTicksPerSecond << endl << endl;

			cout << "		name nodes : " << endl;
			showNodeName(scene->mRootNode);
			cout << endl;

			cout << "		name bones : " << endl;
			processNode(scene->mRootNode, scene);

			cout << "		name nodes animation : " << endl;
			for (GLuint i = 0; i < scene->mAnimations[0]->mNumChannels; i++)
			{
				cout << scene->mAnimations[0]->mChannels[i]->mNodeName.C_Str() << endl;
			}
			cout << endl;
			glBindVertexArray(0);
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

				if (m_bone_mapping.find(bone_name) == m_bone_mapping.end())
				{
					// Allocate an index for a new bone
					bone_index = m_num_bones;
					m_num_bones++;
					BoneMatrix bi;
					m_bone_matrices.push_back(bi);
					m_bone_matrices[bone_index].offset_matrix = mesh->mBones[i]->mOffsetMatrix;
					m_bone_mapping[bone_name] = bone_index;

					//cout << "bone_name: " << bone_name << "			 bone_index: " << bone_index << endl;
				}
				else
				{
					bone_index = m_bone_mapping[bone_name];
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

		const aiNodeAnim* findNodeAnim(const aiAnimation* p_animation, const string p_node_name)
		{
			// channel in animation contains aiNodeAnim (aiNodeAnim its transformation for bones)
			// numChannels == numBones
			for (GLuint i = 0; i < p_animation->mNumChannels; i++)
			{
				const aiNodeAnim* node_anim = p_animation->mChannels[i];
				if (string(node_anim->mNodeName.data) == p_node_name)
				{
					return node_anim;
				}
			}

			return nullptr;
		}

		void readNodeHierarchy(float p_animation_time, const aiNode* p_node, const aiMatrix4x4 parent_transform)
		{

			string node_name(p_node->mName.data);
			const aiAnimation* animation = scene->mAnimations[0];
			aiMatrix4x4 node_transform = p_node->mTransformation;
			const aiNodeAnim* node_anim = findNodeAnim(animation, node_name);

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

				if (string(node_anim->mNodeName.data) == string("Head"))
				{
					aiQuaternion rotate_head = aiQuaternion(rotate_head_xz.w, rotate_head_xz.x, rotate_head_xz.y, rotate_head_xz.z);

					node_transform = translate_matr * (rotate_matr * aiMatrix4x4(rotate_head.GetMatrix())) * scaling_matr;
				}
				else
				{
					node_transform = translate_matr * rotate_matr * scaling_matr;
				}

			}

			aiMatrix4x4 global_transform = parent_transform * node_transform;


			if (m_bone_mapping.find(node_name) != m_bone_mapping.end())
			{
				GLuint bone_index = m_bone_mapping[node_name];
				m_bone_matrices[bone_index].final_world_transform = m_global_inverse_transform * global_transform * m_bone_matrices[bone_index].offset_matrix;
			}

			for (GLuint i = 0; i < p_node->mNumChildren; i++)
			{
				readNodeHierarchy(p_animation_time, p_node->mChildren[i], global_transform);
			}

		}

		void boneTransform(double time_in_sec, vector<aiMatrix4x4>& transforms)
		{
			aiMatrix4x4 identity_matrix; // = mat4(1.0f);
			//identity_matrix[0][0] = 1;
			//identity_matrix.InitIdentity();

			double time_in_ticks = time_in_sec * ticks_per_second;
			float animation_time = fmod(time_in_ticks, scene->mAnimations[0]->mDuration);

			readNodeHierarchy(animation_time, scene->mRootNode, identity_matrix);

			transforms.resize(m_num_bones);

			for (GLuint i = 0; i < m_num_bones; i++)
			{
				transforms[i] = m_bone_matrices[i].final_world_transform;
			}
		}

		glm::mat4 aiToGlm(aiMatrix4x4 ai_matr)
		{
			glm::mat4 result;
			result[0].x = ai_matr.a1; result[0].y = ai_matr.b1; result[0].z = ai_matr.c1; result[0].w = ai_matr.d1;
			result[1].x = ai_matr.a2; result[1].y = ai_matr.b2; result[1].z = ai_matr.c2; result[1].w = ai_matr.d2;
			result[2].x = ai_matr.a3; result[2].y = ai_matr.b3; result[2].z = ai_matr.c3; result[2].w = ai_matr.d3;
			result[3].x = ai_matr.a4; result[3].y = ai_matr.b4; result[3].z = ai_matr.c4; result[3].w = ai_matr.d4;

			//cout << " " << result[0].x << "		 " << result[0].y << "		 " << result[0].z << "		 " << result[0].w << endl;
			//cout << " " << result[1].x << "		 " << result[1].y << "		 " << result[1].z << "		 " << result[1].w << endl;
			//cout << " " << result[2].x << "		 " << result[2].y << "		 " << result[2].z << "		 " << result[2].w << endl;
			//cout << " " << result[3].x << "		 " << result[3].y << "		 " << result[3].z << "		 " << result[3].w << endl;
			//cout << endl;

			//cout << " " << ai_matr.a1 << "		 " << ai_matr.b1 << "		 " << ai_matr.c1 << "		 " << ai_matr.d1 << endl;
			//cout << " " << ai_matr.a2 << "		 " << ai_matr.b2 << "		 " << ai_matr.c2 << "		 " << ai_matr.d2 << endl;
			//cout << " " << ai_matr.a3 << "		 " << ai_matr.b3 << "		 " << ai_matr.c3 << "		 " << ai_matr.d3 << endl;
			//cout << " " << ai_matr.a4 << "		 " << ai_matr.b4 << "		 " << ai_matr.c4 << "		 " << ai_matr.d4 << endl;
			//cout << endl;

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