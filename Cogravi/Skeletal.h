#ifndef SKELETAL_HEADER
#define SKELETAL_HEADER


#include <GL/glew.h>

#include <iostream>
#include <string>
#include <vector>
#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>
#include <glm/gtx/quaternion.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <assimp/scene.h>

#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include <assimp/Importer.hpp>
#include <unordered_map>


typedef unsigned int uint;
typedef unsigned char byte;

inline glm::mat4 assimpToGlmMatrix(aiMatrix4x4 mat) {
	glm::mat4 m;
	for (int y = 0; y < 4; y++)
	{
		for (int x = 0; x < 4; x++)
		{
			m[x][y] = mat[y][x];
		}
	}
	return m;
}
inline glm::vec3 assimpToGlmVec3(aiVector3D vec) {
	return glm::vec3(vec.x, vec.y, vec.z);
}

inline glm::quat assimpToGlmQuat(aiQuaternion quat) {
	glm::quat q;
	q.x = quat.x;
	q.y = quat.y;
	q.z = quat.z;
	q.w = quat.w;

	return q;
}

namespace Cogravi
{

	// vertex of an animated model
	struct Vertexx {
		glm::vec3 position;
		glm::vec3 normal;
		glm::vec2 uv;
		glm::vec4 boneIds = glm::vec4(0);
		glm::vec4 boneWeights = glm::vec4(0.0f);
	};

	// structure to hold bone tree (skeleton)
	struct Bone {
		int id = 0; // position of the bone in final upload array
		std::string name = "";
		glm::mat4 offset = glm::mat4(1.0f);
		std::vector<Bone> children = {};
	};

	// sturction representing an animation track
	struct BoneTransformTrack {
		std::vector<float> positionTimestamps = {};
		std::vector<float> rotationTimestamps = {};
		std::vector<float> scaleTimestamps = {};

		std::vector<glm::vec3> positions = {};
		std::vector<glm::quat> rotations = {};
		std::vector<glm::vec3> scales = {};
	};

	// structure containing animation information
	struct Animation {
		float duration = 0.0f;
		float ticksPerSecond = 1.0f;
		std::unordered_map<std::string, BoneTransformTrack> boneTransforms = {};
	};

	class Skeletal
	{
	public:


		Assimp::Importer importer;

		std::vector<Vertexx> vertices = {};
		std::vector<uint> indices = {};
		uint boneCount = 0;
		Animation animation;
		uint vao = 0;
		Bone skeleton;
		uint diffuseTexture;

		glm::mat4 globalInverseTransform;

		uint viewProjectionMatrixLocation;
		uint modelMatrixLocation;
		uint boneMatricesLocation;
		uint textureLocation;
		glm::mat4 identity;
		uint shader;
		std::vector<glm::mat4> currentPose;

		void load()
		{
			const char* filePath = "assets/animations/man/model.dae";
			const aiScene* scene = importer.ReadFile(filePath, aiProcess_Triangulate | aiProcess_FlipUVs | aiProcess_GenSmoothNormals);

			if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode) {
				std::cout << "ERROR::ASSIMP::" << importer.GetErrorString() << std::endl;
			}
			aiMesh* mesh = scene->mMeshes[0];

			//as the name suggests just inverse the global transform
			this->globalInverseTransform = assimpToGlmMatrix(scene->mRootNode->mTransformation);
			this->globalInverseTransform = glm::inverse(globalInverseTransform);


			loadModel(scene, mesh, vertices, indices, skeleton, boneCount);
			cout << "Bone count: " << boneCount << "\n";
			loadAnimation(scene, animation);

			cout << "Total: " << animation.boneTransforms.size() << "\n";

			//cout << "vertices count: " << vertices.size() << "\n";
			//cout << "indices count: " << indices.size() << "\n";

			vao = createVertexArray(vertices, indices);
			diffuseTexture = createTexture("assets/animations/man/diffuse.png");

			identity = glm::mat4(1.0);

			this->currentPose = {};
			this->currentPose.resize(boneCount, identity); // use this for no animation

			shader = Shader("assets/shaders/animation.vert", "assets/shaders/animation.frag").ID;

			//get all shader uniform locations
			viewProjectionMatrixLocation = glGetUniformLocation(shader, "view_projection_matrix");
			modelMatrixLocation = glGetUniformLocation(shader, "model_matrix");
			boneMatricesLocation = glGetUniformLocation(shader, "bone_transforms");
			textureLocation = glGetUniformLocation(shader, "diff_texture");


		}

		void render(Camera& camera, float animationTime)
		{
			glm::mat4 projectionMatrix = camera.GetProjectionMatrix();
			glm::mat4 viewMatrix = camera.GetViewMatrix();

			glm::mat4 viewProjectionMatrix = projectionMatrix * viewMatrix;

			glm::mat4 modelMatrix(1.0f);
			modelMatrix = glm::translate(modelMatrix, glm::vec3(0.0f, 0.0f, 20.0f));

			modelMatrix = glm::rotate(modelMatrix, glm::radians(-180.0f), glm::vec3(1, 0, 0));
			float dAngle = animationTime * 0.2;
			//modelMatrix = glm::rotate(modelMatrix, dAngle, glm::vec3(0, 1, 0));
			modelMatrix = glm::scale(modelMatrix, glm::vec3(1.0f, 1.0f, 1.0f));

			getPose(animation, skeleton, animationTime, currentPose, identity);

			//glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
			glUseProgram(shader);
			glUniformMatrix4fv(viewProjectionMatrixLocation, 1, GL_FALSE, glm::value_ptr(viewProjectionMatrix));
			glUniformMatrix4fv(modelMatrixLocation, 1, GL_FALSE, glm::value_ptr(modelMatrix));
			glUniformMatrix4fv(boneMatricesLocation, boneCount, GL_FALSE, glm::value_ptr(currentPose[0]));

			glBindVertexArray(vao);
			glActiveTexture(GL_TEXTURE0);
			glBindTexture(GL_TEXTURE_2D, diffuseTexture);
			glUniform1i(textureLocation, 0);

			glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0);


		}

		void render(Avatar& avatar, float animationTime)
		{
			glm::mat4 projectionMatrix = avatar.proj;
			glm::mat4 viewMatrix = avatar.view;

			glm::mat4 viewProjectionMatrix = projectionMatrix * viewMatrix;

			glm::mat4 modelMatrix(1.0f);
			modelMatrix = glm::translate(modelMatrix, glm::vec3(0.0f, 0.0f, 20.0f));

			modelMatrix = glm::rotate(modelMatrix, glm::radians(-180.0f), glm::vec3(1, 0, 0));
			float dAngle = animationTime * 0.2;
			//modelMatrix = glm::rotate(modelMatrix, dAngle, glm::vec3(0, 1, 0));
			modelMatrix = glm::scale(modelMatrix, glm::vec3(1.0f, 1.0f, 1.0f));

			getPose(animation, skeleton, animationTime, currentPose, identity);

			//glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
			glUseProgram(shader);
			glUniformMatrix4fv(viewProjectionMatrixLocation, 1, GL_FALSE, glm::value_ptr(viewProjectionMatrix));
			glUniformMatrix4fv(modelMatrixLocation, 1, GL_FALSE, glm::value_ptr(modelMatrix));
			glUniformMatrix4fv(boneMatricesLocation, boneCount, GL_FALSE, glm::value_ptr(currentPose[0]));

			glBindVertexArray(vao);
			glActiveTexture(GL_TEXTURE0);
			glBindTexture(GL_TEXTURE_2D, diffuseTexture);
			glUniform1i(textureLocation, 0);

			glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0);


		}


		// a recursive function to read all bones and form skeleton
		bool readSkeleton(Bone& boneOutput, aiNode* node, std::unordered_map<std::string, std::pair<int, glm::mat4>>& boneInfoTable) {

			if (boneInfoTable.find(node->mName.C_Str()) != boneInfoTable.end()) { // if node is actually a bone
				boneOutput.name = node->mName.C_Str();
				boneOutput.id = boneInfoTable[boneOutput.name].first;
				boneOutput.offset = boneInfoTable[boneOutput.name].second;

				for (int i = 0; i < node->mNumChildren; i++) {
					Bone child;
					readSkeleton(child, node->mChildren[i], boneInfoTable);
					boneOutput.children.push_back(child);
				}
				return true;
			}
			else { // find bones in children
				for (int i = 0; i < node->mNumChildren; i++) {
					if (readSkeleton(boneOutput, node->mChildren[i], boneInfoTable)) {
						return true;
					}

				}
			}
			return false;
		}

		void loadModel(const aiScene* scene, aiMesh* mesh, std::vector<Vertexx>& verticesOutput, std::vector<uint>& indicesOutput, Bone& skeletonOutput, uint& nBoneCount) {

			verticesOutput = {};
			indicesOutput = {};
			//load position, normal, uv
			for (unsigned int i = 0; i < mesh->mNumVertices; i++) {
				//process position 
				Vertexx vertex;
				glm::vec3 vector;
				vector.x = mesh->mVertices[i].x;
				vector.y = mesh->mVertices[i].y;
				vector.z = mesh->mVertices[i].z;
				vertex.position = vector;
				//process normal
				vector.x = mesh->mNormals[i].x;
				vector.y = mesh->mNormals[i].y;
				vector.z = mesh->mNormals[i].z;
				vertex.normal = vector;
				//process uv
				glm::vec2 vec;
				vec.x = mesh->mTextureCoords[0][i].x;
				vec.y = mesh->mTextureCoords[0][i].y;
				vertex.uv = vec;

				vertex.boneIds = glm::ivec4(0);
				vertex.boneWeights = glm::vec4(0.0f);

				verticesOutput.push_back(vertex);
			}

			//load boneData to vertices
			std::unordered_map<std::string, std::pair<int, glm::mat4>> boneInfo = {};
			std::vector<uint> boneCounts;
			boneCounts.resize(verticesOutput.size(), 0);
			nBoneCount = mesh->mNumBones;

			//loop through each bone
			for (uint i = 0; i < nBoneCount; i++) {
				aiBone* bone = mesh->mBones[i];
				glm::mat4 m = assimpToGlmMatrix(bone->mOffsetMatrix);
				boneInfo[bone->mName.C_Str()] = { i, m };

				//loop through each vertex that have that bone
				for (int j = 0; j < bone->mNumWeights; j++) {
					uint id = bone->mWeights[j].mVertexId;
					float weight = bone->mWeights[j].mWeight;
					boneCounts[id]++;
					switch (boneCounts[id]) {
					case 1:
						verticesOutput[id].boneIds.x = i;
						verticesOutput[id].boneWeights.x = weight;
						break;
					case 2:
						verticesOutput[id].boneIds.y = i;
						verticesOutput[id].boneWeights.y = weight;
						break;
					case 3:
						verticesOutput[id].boneIds.z = i;
						verticesOutput[id].boneWeights.z = weight;
						break;
					case 4:
						verticesOutput[id].boneIds.w = i;
						verticesOutput[id].boneWeights.w = weight;
						break;
					default:
						//std::cout << "err: unable to allocate bone to vertex" << std::endl;
						break;

					}
				}
			}

			//normalize weights to make all weights sum 1
			for (int i = 0; i < verticesOutput.size(); i++) {
				glm::vec4& boneWeights = verticesOutput[i].boneWeights;
				float totalWeight = boneWeights.x + boneWeights.y + boneWeights.z + boneWeights.w;
				if (totalWeight > 0.0f) {
					verticesOutput[i].boneWeights = glm::vec4(
						boneWeights.x / totalWeight,
						boneWeights.y / totalWeight,
						boneWeights.z / totalWeight,
						boneWeights.w / totalWeight
					);
				}
			}

			//load indices
			for (int i = 0; i < mesh->mNumFaces; i++) {
				aiFace& face = mesh->mFaces[i];
				for (unsigned int j = 0; j < face.mNumIndices; j++)
					indicesOutput.push_back(face.mIndices[j]);
			}

			// create bone hirerchy
			readSkeleton(skeletonOutput, scene->mRootNode, boneInfo);
		}

		void loadAnimation(const aiScene* scene, Animation& animation) {
			//loading  first Animation
			aiAnimation* anim = scene->mAnimations[0];

			if (anim->mTicksPerSecond != 0.0f)
				animation.ticksPerSecond = anim->mTicksPerSecond;
			else
				animation.ticksPerSecond = 1;


			animation.duration = anim->mDuration * anim->mTicksPerSecond;
			animation.boneTransforms = {};

			//load positions rotations and scales for each bone
			// each channel represents each bone
			for (int i = 0; i < anim->mNumChannels; i++) {
				aiNodeAnim* channel = anim->mChannels[i];
				cout << "Nodo: " << channel->mNodeName.data << "\n";
				BoneTransformTrack track;
				for (int j = 0; j < channel->mNumPositionKeys; j++) {
					track.positionTimestamps.push_back(channel->mPositionKeys[j].mTime);
					track.positions.push_back(assimpToGlmVec3(channel->mPositionKeys[j].mValue));
				}
				for (int j = 0; j < channel->mNumRotationKeys; j++) {
					track.rotationTimestamps.push_back(channel->mRotationKeys[j].mTime);
					track.rotations.push_back(assimpToGlmQuat(channel->mRotationKeys[j].mValue));

				}
				for (int j = 0; j < channel->mNumScalingKeys; j++) {
					track.scaleTimestamps.push_back(channel->mScalingKeys[j].mTime);
					track.scales.push_back(assimpToGlmVec3(channel->mScalingKeys[j].mValue));

				}
				animation.boneTransforms[channel->mNodeName.C_Str()] = track;
				cout << "Size: " << animation.boneTransforms.size() << "\n";
			}
		}

		unsigned int createVertexArray(std::vector<Vertexx>& vertices, std::vector<uint> indices) {
			uint
				vao = 0,
				vbo = 0,
				ebo = 0;

			glGenVertexArrays(1, &vao);
			glGenBuffers(1, &vbo);
			glGenBuffers(1, &ebo);

			glBindVertexArray(vao);
			glBindBuffer(GL_ARRAY_BUFFER, vbo);
			glBufferData(GL_ARRAY_BUFFER, sizeof(Vertexx) * vertices.size(), &vertices[0], GL_STATIC_DRAW);
			glEnableVertexAttribArray(0);
			glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertexx), (GLvoid*)offsetof(Vertexx, position));
			glEnableVertexAttribArray(1);
			glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertexx), (GLvoid*)offsetof(Vertexx, normal));
			glEnableVertexAttribArray(2);
			glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertexx), (GLvoid*)offsetof(Vertexx, uv));
			glEnableVertexAttribArray(3);
			glVertexAttribPointer(3, 4, GL_FLOAT, GL_FALSE, sizeof(Vertexx), (GLvoid*)offsetof(Vertexx, boneIds));
			glEnableVertexAttribArray(4);
			glVertexAttribPointer(4, 4, GL_FLOAT, GL_FALSE, sizeof(Vertexx), (GLvoid*)offsetof(Vertexx, boneWeights));

			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
			glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(uint), &indices[0], GL_STATIC_DRAW);
			glBindVertexArray(0);
			return vao;
		}

		uint createTexture(std::string filepath) {
			uint textureId = 0;
			int width, height, nrChannels;
			byte* data = stbi_load(filepath.c_str(), &width, &height, &nrChannels, 4);
			glGenTextures(1, &textureId);
			glBindTexture(GL_TEXTURE_2D, textureId);

			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAX_LEVEL, 3);

			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);

			stbi_image_free(data);
			glBindTexture(GL_TEXTURE_2D, 0);
			return textureId;
		}



		std::pair<uint, float> getTimeFraction(std::vector<float>& times, float& dt) {
			uint segment = 0;
			while (dt > times[segment])
				segment++;
			//cout << "count: " << times.size() << "\n";
			float start = times[segment - 1];
			float end = times[segment];
			float frac = (dt - start) / (end - start);

			/*cout << "start: " << start << "\n";
			cout << "end: " << end << "\n";
			cout << "frac: " << frac << "\n";*/

			return { segment, frac };
		}



		void getPose(Animation& animation, Bone& skeletion, float dt, std::vector<glm::mat4>& output, glm::mat4& parentTransform) {

			BoneTransformTrack& btt = animation.boneTransforms[skeletion.name];
			dt = fmod(dt, animation.duration);
			std::pair<uint, float> fp;
			//calculate interpolated position
			//cout << "count pos1: " << btt.positions.size() << "\n";
			fp = getTimeFraction(btt.positionTimestamps, dt);

			glm::vec3 position1 = btt.positions[fp.first - 1];
			glm::vec3 position2 = btt.positions[fp.first];

			glm::vec3 position = glm::mix(position1, position2, fp.second);

			//calculate interpolated rotation
			fp = getTimeFraction(btt.rotationTimestamps, dt);
			glm::quat rotation1 = btt.rotations[fp.first - 1];
			glm::quat rotation2 = btt.rotations[fp.first];

			glm::quat rotation = glm::slerp(rotation1, rotation2, fp.second);

			//calculate interpolated scale
			fp = getTimeFraction(btt.scaleTimestamps, dt);
			glm::vec3 scale1 = btt.scales[fp.first - 1];
			glm::vec3 scale2 = btt.scales[fp.first];

			glm::vec3 scale = glm::mix(scale1, scale2, fp.second);

			glm::mat4 positionMat = glm::mat4(1.0),
				scaleMat = glm::mat4(1.0);


			// calculate localTransform
			positionMat = glm::translate(positionMat, position);
			glm::mat4 rotationMat = glm::toMat4(rotation);
			scaleMat = glm::scale(scaleMat, scale);
			glm::mat4 localTransform = positionMat * rotationMat * scaleMat;
			glm::mat4 globalTransform = parentTransform * localTransform;

			//output[0] = globalInverseTransform * globalTransform * skeletion.offset;
			output[skeletion.id] = this->globalInverseTransform * globalTransform * skeletion.offset;

			//cout << "size children: " << skeletion.children.size() << "\n";

			////update values for children bones
			for (Bone& child : skeletion.children) {
				getPose(animation, child, dt, output, globalTransform);
			}
			//getPose(animation, skeletion.children[0], dt, output, globalTransform, globalInverseTransform);
			//std::cout << dt << " => " << position.x << ":" << position.y << ":" << position.z << ":" << std::endl;
		}



	};
}

#endif
