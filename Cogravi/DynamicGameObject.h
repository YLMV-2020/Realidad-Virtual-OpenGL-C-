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

		unordered_map<string, GLuint> mapBone;
		GLuint numBones = 0;
		vector<BoneMatrix> matrixBone;
		aiMatrix4x4 matrixInverse;

		GLuint boneID;
		float ticksPerSecond = 0.0f;

	    unordered_map<string, const aiNodeAnim*> mapNodeAnim[MAX_ANIMATIONS];

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


		virtual void renderInstance(Camera& camera, Shader& shader, float animationTime)
		{
			glm::mat4 projection = camera.GetProjectionMatrix();
			glm::mat4 view = camera.GetViewMatrix();
			shader.use();
			shader.setMat4("projection", projection);
			shader.setMat4("view", view);

			drawInstance(shader, animationTime);
		}

		virtual void renderInstance(Avatar& avatar, Shader& shader, float animationTime)
		{
			glm::mat4 projection = avatar.GetProjectionMatrix();
			glm::mat4 view = avatar.GetViewMatrix();
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
				model = glm::scale(model, glm::vec3(0.01f));

				// 3. rotation: add random rotation around a (semi)randomly picked rotation axis vector
				float rotAngle = (rand() % 360);
				model = glm::rotate(model, rotAngle, glm::vec3(0.0f, 1.f, 0.f));

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

			shader.setMat4("projection", projection);
			shader.setMat4("view", view);
			shader.setMat4("model", transform);
		
			//glm::mat4 matr_normals_cube = glm::mat4(glm::transpose(glm::inverse(transform)));
		
			draw(shader, animationTime);
		}

		virtual void render(Avatar& avatar, Shader& shader, float animationTime)
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

			shader.setMat4("projection", projection);
			shader.setMat4("view", view);
			shader.setMat4("model", transform);

			//glm::mat4 matr_normals_cube = glm::mat4(glm::transpose(glm::inverse(transform)));

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

			for (GLuint i = 0; i < scene[numAnimations]->mNumAnimations; i++)
			{
				const aiAnimation* animation = scene[numAnimations]->mAnimations[i];
				animations.push_back(animation);
				root.push_back(scene[numAnimations]->mRootNode);
				int index = animations.size() - 1;
				for (GLuint j = 0; j < animation->mNumChannels; j++)
				{
					const aiNodeAnim* node_anim = animation->mChannels[j];
					mapNodeAnim[index][string(node_anim->mNodeName.data)] = node_anim;
				}
			}	
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
			//cout << "Num de animaciones: " << scene[0]->mNumAnimations << "\n";

			loadAnimations();

			directory = path.substr(0, path.find_last_of('/'));
			//cout << "Direc: " << directory << "\n";

			//cout << "scene->HasAnimations() 1: " << scene[0]->HasAnimations() << endl;
			//cout << "scene->mNumMeshes 1: " << scene[0]->mNumMeshes << endl;
			//cout << "scene[0]->mAnimations[0]->mNumChannels 1: " << scene[0]->mAnimations[0]->mNumChannels << endl;
			//cout << "scene[0]->mAnimations[0]->mDuration 1: " << scene[0]->mAnimations[0]->mDuration << endl;
			//cout << "scene[0]->mAnimations[0]->mTicksPerSecond 1: " << scene[0]->mAnimations[0]->mTicksPerSecond << endl << endl;

			//cout << "		name nodes : " << endl;
			//showNodeName(scene[0]->mRootNode);
			//cout << endl;

			//cout << "		name bones : " << endl;
			processNode(scene[0]->mRootNode, scene[0]);

			//cout << "		name nodes animation : " << endl;
			/*for (GLuint i = 0; i < scene[0]->mAnimations[0]->mNumChannels; i++)
			{
				cout << scene[0]->mAnimations[0]->mChannels[i]->mNodeName.C_Str() << endl;
			}
			cout << endl;*/
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
				aiMesh* aiMesh = scene->mMeshes[i];
				mesh = processMesh(aiMesh, scene);
				meshes.push_back(mesh); 
			}
		}

		MeshA processMesh(aiMesh* mesh, const aiScene* scene)
		{
			vector<VertexA> vertices;
			vector<GLuint> indices;
			vector<Texture> textures;
			vector<VertexBoneData> bones;

			vertices.reserve(mesh->mNumVertices);
			indices.reserve(mesh->mNumVertices);
			bones.resize(mesh->mNumVertices);

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

			for (GLuint i = 0; i < mesh->mNumFaces; i++)
			{
				aiFace face = mesh->mFaces[i];
				indices.push_back(face.mIndices[0]);
				indices.push_back(face.mIndices[1]);
				indices.push_back(face.mIndices[2]);
			}

			for (GLuint i = 0; i < mesh->mNumBones; i++)
			{
				GLuint bone_index = 0;
				string bone_name(mesh->mBones[i]->mName.data);

				//cout << mesh->mBones[i]->mName.data << endl;

				if (mapBone.find(bone_name) == mapBone.end())
				{
					bone_index = numBones;
					numBones++;
					BoneMatrix bi;
					matrixBone.push_back(bi);
					matrixBone[bone_index].offsetMatrix = mesh->mBones[i]->mOffsetMatrix;
					mapBone[bone_name] = bone_index;
				}
				else
				{
					bone_index = mapBone[bone_name];
				}

				for (GLuint j = 0; j < mesh->mBones[i]->mNumWeights; j++)
				{
					GLuint vertex_id = mesh->mBones[i]->mWeights[j].mVertexId;
					float weight = mesh->mBones[i]->mWeights[j].mWeight;
					bones[vertex_id].addBoneData(bone_index, weight);
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

				return MeshA(vertices, indices, textures, bones);
			}

			return MeshA(vertices, indices, this->textures, bones);
					
		}

		vector<Texture> loadMaterialTextures(aiMaterial* material, aiTextureType type, TextureType typeName)
		{
			vector<Texture> textures;
			for (unsigned int i = 0; i < material->GetTextureCount(type); i++)
			{
				aiString str;
				material->GetTexture(type, i, &str);

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

		GLuint findPosition(float animationTime, const aiNodeAnim* nodeAnim)
		{
			for (GLuint i = 0; i < nodeAnim->mNumPositionKeys - 1; i++)
			{
				if (animationTime < (float)nodeAnim->mPositionKeys[i + 1].mTime)
				{
					return i;
				}
			}
			assert(0);
			return 0;
		}

		GLuint findRotation(float animationTime, const aiNodeAnim* nodeAnim)
		{
			assert(nodeAnim->mNumRotationKeys > 0);
			for (GLuint i = 0; i < nodeAnim->mNumRotationKeys - 1; i++)
			{
				if (animationTime < (float)nodeAnim->mRotationKeys[i + 1].mTime)
				{
					return i;
				}
			}
			assert(0);
			return 0;
		}

		GLuint findScaling(float animationTime, const aiNodeAnim* nodeAnim)
		{
			for (GLuint i = 0; i < nodeAnim->mNumScalingKeys - 1; i++)
			{
				if (animationTime < (float)nodeAnim->mScalingKeys[i + 1].mTime)
				{
					return i;
				}
			}
			assert(0);
			return 0;
		}

		aiVector3D calcInterpolatedPosition(float animationTime, const aiNodeAnim* nodeAnim)
		{
			if (nodeAnim->mNumPositionKeys == 1)
				return nodeAnim->mPositionKeys[0].mValue;
			
			GLuint position_index = findPosition(animationTime, nodeAnim);
			GLuint next_position_index = position_index + 1;
			assert(next_position_index < nodeAnim->mNumPositionKeys);

			float deltaTime = (float)(nodeAnim->mPositionKeys[next_position_index].mTime - nodeAnim->mPositionKeys[position_index].mTime);
			float factor = (animationTime - (float)nodeAnim->mPositionKeys[position_index].mTime) / deltaTime;

			assert(factor >= 0.0f && factor <= 1.0f);
			aiVector3D start = nodeAnim->mPositionKeys[position_index].mValue;
			aiVector3D end = nodeAnim->mPositionKeys[next_position_index].mValue;
			aiVector3D delta = end - start;

			return start + factor * delta;
		}

		aiQuaternion calcInterpolatedRotation(float animationTime, const aiNodeAnim* nodeAnim)
		{
			if (nodeAnim->mNumRotationKeys == 1)
				return nodeAnim->mRotationKeys[0].mValue;

			GLuint rotation_index = findRotation(animationTime, nodeAnim);
			GLuint next_rotation_index = rotation_index + 1;
			assert(next_rotation_index < nodeAnim->mNumRotationKeys);

			float deltaTime = (float)(nodeAnim->mRotationKeys[next_rotation_index].mTime - nodeAnim->mRotationKeys[rotation_index].mTime);
			float factor = (animationTime - (float)nodeAnim->mRotationKeys[rotation_index].mTime) / deltaTime;

			assert(factor >= 0.0f && factor <= 1.0f);
			aiQuaternion startQuat = nodeAnim->mRotationKeys[rotation_index].mValue;
			aiQuaternion endQuat = nodeAnim->mRotationKeys[next_rotation_index].mValue;

			return nlerp(startQuat, endQuat, factor);
		}

		aiVector3D calcInterpolatedScaling(float animationTime, const aiNodeAnim* nodeAnim)
		{
			if (nodeAnim->mNumScalingKeys == 1)
				return nodeAnim->mScalingKeys[0].mValue;

			GLuint scaling_index = findScaling(animationTime, nodeAnim);
			GLuint next_scaling_index = scaling_index + 1;
			assert(next_scaling_index < nodeAnim->mNumScalingKeys);

			float deltaTime = (float)(nodeAnim->mScalingKeys[next_scaling_index].mTime - nodeAnim->mScalingKeys[scaling_index].mTime);
			float  factor = (animationTime - (float)nodeAnim->mScalingKeys[scaling_index].mTime) / deltaTime;

			assert(factor >= 0.0f && factor <= 1.0f);
			aiVector3D start = nodeAnim->mScalingKeys[scaling_index].mValue;
			aiVector3D end = nodeAnim->mScalingKeys[next_scaling_index].mValue;
			aiVector3D delta = end - start;

			return start + factor * delta;
		}

		void readNodeHierarchy(float animationTime, const aiNode* node, const aiMatrix4x4 parentTransform)
		{
			string nodeName(node->mName.data);
			const aiAnimation* animation = animations[currentAnimation];
			aiMatrix4x4 nodeTransform = node->mTransformation;
			const aiNodeAnim* nodeAnim = mapNodeAnim[currentAnimation][nodeName];

			if (nodeAnim)
			{

				aiVector3D scale = calcInterpolatedScaling(animationTime, nodeAnim);
				aiMatrix4x4 matrixScale;
				aiMatrix4x4::Scaling(scale, matrixScale);

				aiQuaternion rotation = calcInterpolatedRotation(animationTime, nodeAnim);
				aiMatrix4x4 matrixRotation = aiMatrix4x4(rotation.GetMatrix());

				aiVector3D translate = calcInterpolatedPosition(animationTime, nodeAnim);
				aiMatrix4x4 matrixTranslate;
				aiMatrix4x4::Translation(translate, matrixTranslate);

				nodeTransform = matrixTranslate * matrixRotation * matrixScale;

			}

			aiMatrix4x4 globalTransform = parentTransform * nodeTransform;

			if (mapBone.find(nodeName) != mapBone.end())
			{
				GLuint boneIndex = mapBone[nodeName];
				matrixBone[boneIndex].finalWorldTransform = matrixInverse * globalTransform * matrixBone[boneIndex].offsetMatrix;
			}

			for (GLuint i = 0; i < node->mNumChildren; i++)
				readNodeHierarchy(animationTime, node->mChildren[i], globalTransform);
			
		}

		void boneTransform(double second, vector<aiMatrix4x4>& transforms)
		{
			aiMatrix4x4 identityMatrix; 

			const aiAnimation* animation = animations[currentAnimation];

			if (animation->mTicksPerSecond != 0.0)
				ticksPerSecond = animation->mTicksPerSecond;			
			else
				ticksPerSecond = 25.0f;
		
			double timeInTicks = second * ticksPerSecond;
			float animationTime = fmod(timeInTicks, animation->mDuration);

			readNodeHierarchy(animationTime, root[currentAnimation], identityMatrix);

			transforms.resize(numBones);

			for (GLuint i = 0; i < numBones; i++)
				transforms[i] = matrixBone[i].finalWorldTransform;
			
		}

		glm::mat4 aiToGlm(aiMatrix4x4 matrix)
		{
			glm::mat4 result;
			result[0].x = matrix.a1; result[0].y = matrix.b1; result[0].z = matrix.c1; result[0].w = matrix.d1;
			result[1].x = matrix.a2; result[1].y = matrix.b2; result[1].z = matrix.c2; result[1].w = matrix.d2;
			result[2].x = matrix.a3; result[2].y = matrix.b3; result[2].z = matrix.c3; result[2].w = matrix.d3;
			result[3].x = matrix.a4; result[3].y = matrix.b4; result[3].z = matrix.c4; result[3].w = matrix.d4;

			return result;
		}

		aiQuaternion nlerp(aiQuaternion a, aiQuaternion b, float blend)
		{
			a.Normalize();
			b.Normalize();

			aiQuaternion result;
			float dotProduct = a.x * b.x + a.y * b.y + a.z * b.z + a.w * b.w;
			float oneMinusBlend = 1.0f - blend;

			if (dotProduct < 0.0f)
			{
				result.x = a.x * oneMinusBlend + blend * -b.x;
				result.y = a.y * oneMinusBlend + blend * -b.y;
				result.z = a.z * oneMinusBlend + blend * -b.z;
				result.w = a.w * oneMinusBlend + blend * -b.w;
			}
			else
			{
				result.x = a.x * oneMinusBlend + blend * b.x;
				result.y = a.y * oneMinusBlend + blend * b.y;
				result.z = a.z * oneMinusBlend + blend * b.z;
				result.w = a.w * oneMinusBlend + blend * b.w;
			}

			return result.Normalize();
		}

	};
}
#endif
