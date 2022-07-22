#pragma once
#include <glad/glad.h>

#include <glm/glm.hpp>
#include <stb_image.h>
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include <render/mesh.h>
#include <render/shader.h>
#include <render/animation/animdata.h>

#include <string>
#include <iostream>
#include <map>
#include <vector>
#include "assimp_glm_helpers.h"
#include "utils/log.h"

namespace FlatEngine {
	struct MeshRendererComponent;

	class Model {
	public:
		std::vector<Texture> textures_loaded;
		std::vector<glm::vec4> colors;
		std::vector<Mesh> meshes;
		std::string directory;
		std::string m_Path;
		std::string name = "model";
		bool gammaCorrection;
	
		Model(std::string const& path, bool gamma = false);
		void Draw(Shader& shader, const MeshRendererComponent& mrc);
	
		auto& GetBoneInfoMap() { return m_BoneInfoMap; }
		int& GetBoneCount() { return m_BoneCounter; }
		
	private:
		std::map<std::string, BoneInfo> m_BoneInfoMap;
		int m_BoneCounter = 0;

		void loadModel(std::string const& path) {
			Assimp::Importer importer;
			const aiScene* scene = importer.ReadFile(path, aiProcess_Triangulate | aiProcess_GenSmoothNormals | aiProcess_FlipUVs | aiProcess_CalcTangentSpace);
			if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode) // if is Not Zero
			{
				FE_LOG_ERROR("Model importer error: {}",importer.GetErrorString());
				return;
			}
			m_Path = path;
			directory = path.substr(0, path.find_last_of('/'));
			name = path.substr(path.find_last_of("/\\") + 1);

			processNode(scene->mRootNode, scene);
		}

		void processNode(aiNode* node, const aiScene* scene) {
			for (unsigned int i = 0; i < node->mNumMeshes; i++) {
				aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
				meshes.push_back(processMesh(mesh, scene));
			}
			for (unsigned int i = 0; i < node->mNumChildren; i++) {
				processNode(node->mChildren[i], scene);
			}

		}

		void SetVertexBoneDataToDefault(Vertex& vertex) {
			for (int i = 0; i < MAX_BONE_INFLUENCE; i++) {
				vertex.m_BoneIDs[i] = -1;
				vertex.m_Weights[i] = 0.0f;
			}
		}

		Mesh processMesh(aiMesh* mesh, const aiScene* scene) {
			std::vector<Vertex> vertices;
			std::vector<unsigned int> indices;
			std::vector<Texture> textures;

			for (unsigned int i = 0; i < mesh->mNumVertices; i++) {
				Vertex vertex;
				SetVertexBoneDataToDefault(vertex);
				vertex.Position = AssimpGLMHelpers::GetGLMVec(mesh->mVertices[i]);
				if (mesh->HasNormals()) {
					vertex.Normal = AssimpGLMHelpers::GetGLMVec(mesh->mNormals[i]);
				}
				if (mesh->mTextureCoords[0])
				{
					glm::vec3 vector;
					glm::vec2 vec;
					vec.x = mesh->mTextureCoords[0][i].x;
					vec.y = mesh->mTextureCoords[0][i].y;
					vertex.TexCoords = vec;
					// tangent
					vector.x = mesh->mTangents[i].x;
					vector.y = mesh->mTangents[i].y;
					vector.z = mesh->mTangents[i].z;
					vertex.Tangent = vector;
					// bitangent
					vector.x = mesh->mBitangents[i].x;
					vector.y = mesh->mBitangents[i].y;
					vector.z = mesh->mBitangents[i].z;
					vertex.Bitangent = vector;
				}
				else
					vertex.TexCoords = glm::vec2(0.0f, 0.0f);

				vertices.push_back(vertex);
			}
			for (unsigned int i = 0; i < mesh->mNumFaces; i++) {
				aiFace face = mesh->mFaces[i];
				for (unsigned int j = 0; j < face.mNumIndices; j++)
					indices.push_back(face.mIndices[j]);
			}
			aiMaterial* material = scene->mMaterials[mesh->mMaterialIndex];
			// diffuse: texture_diffuseN
			// specular: texture_specularN
			// normal: texture_normalN

			std::vector<Texture> diffuseMaps = loadMaterialTextures(material, aiTextureType_DIFFUSE, "texture_diffuse");
			textures.insert(textures.end(), diffuseMaps.begin(), diffuseMaps.end());

			std::vector<Texture> specularMaps = loadMaterialTextures(material, aiTextureType_SPECULAR, "texture_specular");
			textures.insert(textures.end(), specularMaps.begin(), specularMaps.end());

			std::vector<Texture> normalMaps = loadMaterialTextures(material, aiTextureType_HEIGHT, "texture_normal");
			textures.insert(textures.end(), normalMaps.begin(), normalMaps.end());

			std::vector<Texture> heightMaps = loadMaterialTextures(material, aiTextureType_AMBIENT, "texture_height");
			textures.insert(textures.end(), heightMaps.begin(), heightMaps.end());

			loadMaterialColors(material);

			ExtractBoneWeightForVertices(vertices, mesh, scene);

			return Mesh(vertices, indices, textures, colors);
		}

		void SetVertexBoneData(Vertex& vertex, int boneID, float weight) {
			for (int i = 0; i < MAX_BONE_INFLUENCE; ++i) {
				if (vertex.m_BoneIDs[i] < 0) {
					vertex.m_Weights[i] = weight;
					vertex.m_BoneIDs[i] = boneID;
					break;
				}
			}
		}

		void ExtractBoneWeightForVertices(std::vector<Vertex>& vertices, aiMesh* mesh, const aiScene* scene) {
			auto& boneInfoMap = m_BoneInfoMap;
			int& boneCount = m_BoneCounter;

			for (int boneIndex = 0; boneIndex < mesh->mNumBones; ++boneIndex) {
				int boneID = -1;
				std::string boneName = mesh->mBones[boneIndex]->mName.C_Str();
				if (boneInfoMap.find(boneName) == boneInfoMap.end()) {
					BoneInfo newBoneInfo;
					newBoneInfo.id = boneCount;
					newBoneInfo.offset = AssimpGLMHelpers::ConvertMatrixToGLMFormat(mesh->mBones[boneIndex]->mOffsetMatrix);
					boneInfoMap[boneName] = newBoneInfo;
					boneID = boneCount;
					boneCount++;
				}
				else {
					boneID = boneInfoMap[boneName].id;
				}
				assert(boneID != -1);
				auto weights = mesh->mBones[boneIndex]->mWeights;
				int numWeights = mesh->mBones[boneIndex]->mNumWeights;

				for (int weightIndex = 0; weightIndex < numWeights; ++weightIndex) {
					int vertexId = weights[weightIndex].mVertexId;
					float weight = weights[weightIndex].mWeight;
					assert(vertexId <= vertices.size());
					SetVertexBoneData(vertices[vertexId], boneID, weight);
				}
			}
		}

		unsigned int TextureFromFile(const char* path, const std::string& directory, bool gamma) {
			std::string filename = std::string(path);
			filename = directory + '/' + filename;

			unsigned int textureID;
			glGenTextures(1, &textureID);

			int width, height, nrComponents;
			unsigned char* data = stbi_load(filename.c_str(), &width, &height, &nrComponents, 0);
			if (data) {
				GLenum format = 0;
				GLenum internalFormat = 0;
				if (nrComponents == 1)
					format = GL_RED;
				else if (nrComponents == 3) {
					internalFormat = gammaCorrection ? GL_SRGB : GL_RGB;
					format = GL_RGB;
				}
				else if (nrComponents == 4) {
					format = GL_RGBA;
					internalFormat = gammaCorrection ? GL_SRGB_ALPHA : GL_RGBA;
				}

				glBindTexture(GL_TEXTURE_2D, textureID);
				glTexImage2D(GL_TEXTURE_2D, 0, internalFormat, width, height, 0, format, GL_UNSIGNED_BYTE, data);
				glGenerateMipmap(GL_TEXTURE_2D);

				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

				stbi_image_free(data);
			}
			else {
				FE_LOG_ERROR("Texture failed to load at path: {}", path);
				stbi_image_free(data);
			}

			return textureID;
		}
		void loadMaterialColors(aiMaterial* mat){
			glm::vec4 diffuseColor = glm::vec4(1.0f);
			aiColor4D diffuse;
			if (AI_SUCCESS == aiGetMaterialColor(mat, AI_MATKEY_COLOR_DIFFUSE, &diffuse))
				diffuseColor = glm::vec4(diffuse.r, diffuse.g, diffuse.b, diffuse.a);
			colors.emplace_back(diffuseColor);
		}
		std::vector<Texture> loadMaterialTextures(aiMaterial* mat, aiTextureType type, std::string typeName) {
			std::vector<Texture> textures;
			for (unsigned int i = 0; i < mat->GetTextureCount(type); i++) {
				aiString str;
				mat->GetTexture(type, i, &str);
				bool skip = false;
				for (unsigned int j = 0; j < textures_loaded.size(); j++) {
					if (std::strcmp(textures_loaded[j].m_Path.data(), str.C_Str()) == 0) {
						textures.push_back(textures_loaded[j]);
						skip = true;
						break;
					}
				}
				if (!skip) {
					Texture texture;
					texture.m_ID = TextureFromFile(str.C_Str(), this->directory, true);
					texture.m_Type = typeName;
					texture.m_Path = str.C_Str();
					textures.push_back(texture);
					textures_loaded.push_back(texture);
					FE_LOG_INFO("Texture loaded id: {} - name: {}", texture.m_ID, str.C_Str());
				}
			}
			return textures;
		}
	};
}
