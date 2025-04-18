#pragma once

#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/type_ptr.hpp"
#include "glm/gtc/quaternion.hpp"

#include "stdEx/utilityEx.h"

#include <functional>
#include <vector>
#include <string>
#include <array>
#include <unordered_map>

#include "TreeManager.h"

namespace LGLStructs
{
	struct BasicVertex
	{
		enum class BasicVertexData
		{
			Position,
			Normal,
			TexCoords,
			Tangent,
			Bitangent,
			_SIZE
		};
		
		glm::vec3 Position;
		glm::vec3 Normal;
		glm::vec3 TexCoords;
		glm::vec3 Tangent;
		glm::vec3 Bitangent;

		glm::vec3& operator[](const size_t index)
		{
			BasicVertexData vertData = static_cast<BasicVertexData>(index);

			switch (vertData)
			{
			case BasicVertexData::Position:
				return Position;
			case BasicVertexData::Normal:
				return Normal;
			case BasicVertexData::TexCoords:
				return TexCoords;
			case BasicVertexData::Tangent:
				return Tangent;
			case BasicVertexData::Bitangent:
				return Bitangent;
			default:
				assert(false && "Invaling index in VertexData");
			}
		}

		constexpr static size_t GetLocalMemberAmount()
		{
			constexpr size_t memberAmount = static_cast<size_t>(BasicVertexData::_SIZE);
			constexpr size_t memberAmountDoubleCheck = sizeof(BasicVertex) / sizeof(glm::vec3);
			static_assert(memberAmount == memberAmountDoubleCheck, "Member amount and enum mismatch");

			return memberAmount;
		}

		constexpr static size_t GetMemberElementSize()
		{
			return glm::vec3::length();
		}
	};


	struct Vertex : BasicVertex
	{
		constexpr static size_t maxWeightPerVertex = 4;

		enum class VertexData
		{
			BoneID,
			Weights,
			_SIZE
		};

		std::array<int, maxWeightPerVertex> boneIDs{};
		std::array<float, maxWeightPerVertex>  boneWeights{};

		void AddBoneData(unsigned int boneID, float boneWeight)
		{
			for (size_t i = 0; i < maxWeightPerVertex; ++i)
			{
				if (boneWeights[i] == 0.0f)
				{
					boneIDs[i] = boneID;
					boneWeights[i] = boneWeight;

					return;
				}
			}

			//assert(false && "Attempt to add more than 4 weights to vertex");
		}

		void NormalizeIfEmptyWeights()
		{
			float sum = 0.0f;
			
			for (auto weight : boneWeights)
			{
				if (weight > 0.0f)
				{
					return;
				}
			}

			boneWeights[0] = 1.0f;
		}

		constexpr static size_t GetLocalMemberAmount()
		{
			// Reconfirmation of equivalency of float and int raw sizes
			static_assert(sizeof(float) == sizeof(int), "Unexpected diffs in float and uint sizes");

			constexpr size_t memberAmount = static_cast<size_t>(VertexData::_SIZE);
			constexpr size_t memberAmountDoubleCheck =
				(sizeof(Vertex) - sizeof(BasicVertex)) / sizeof(std::array<float, maxWeightPerVertex>);
			static_assert(memberAmount == memberAmountDoubleCheck, "Member amount and enum mismatch");

			return memberAmount;
		}

		constexpr static size_t GetMemberElementSize()
		{
			return maxWeightPerVertex;
		}
	};

	struct Texture
	{
		using TextureData = unsigned char*;

		enum class TextureType
		{
			Diffuse,
			Specular,
			Normal,
			Height,
			_SIZE
		};

		struct TextureParams
		{
			enum class TextureOverlayType
			{
				Repeat,
				Mirrored,
				EdgeClamp,
				BorderClamp
			};

			struct BilinearFiltrationConfig
			{
				bool minFilter = false;
				bool maxFilter = true;
			};

			glm::vec4 color = { 1.0f, 1.0f, 1.0f, 1.0f };
			TextureOverlayType overlay = TextureOverlayType::Repeat;
			BilinearFiltrationConfig BFConfig = {};
			bool createMipmaps = false;
			BilinearFiltrationConfig mipmapBFConfig = {};
		};

		std::string name;
		TextureType type = TextureType::Diffuse;
		TextureData data = nullptr;
		TextureParams params;
		int width;
		int height;
		int channelAmount;

		constexpr static size_t GetTextureTypeAmount()
		{
			constexpr size_t typeAmount = static_cast<size_t>(TextureType::_SIZE);

			return typeAmount;
		}
	};

	struct Mesh
	{
		std::vector<Vertex> vert;
		std::vector<unsigned int> indices;
		std::vector<Texture> textures;
	};

	struct MeshInfo
	{
		Mesh mesh;
		stdEx::ValWithBackup<bool> isDynamic;
		stdEx::ValWithBackup<bool> render;
		stdEx::ValWithBackup<std::string> shaderProgram;
		stdEx::ValWithBackup<std::function<void()>> behaviour;

		MeshInfo(const Mesh& mesh, bool& render, bool& isDynamic, std::string& shaderProgram, std::function<void()>& behaviour)
			: mesh(mesh), render(&render), isDynamic(&isDynamic), shaderProgram(&shaderProgram), behaviour(&behaviour) {}

	};
	
	struct ModelInfo
	{
		struct BoneInfo
		{
			int id = -1;
			glm::mat4 offsetMatrix = glm::mat4(1.0f);
			glm::mat4 localTransform = glm::mat4(1.0f);
			glm::mat4 globalTransform = glm::mat4(1.0f);
			glm::mat4 finalTransform = glm::mat4(1.0f);
		};

		struct AnimInfo
		{
			constexpr static double defaultTicksPerSecond = 25;

			double animDuration;
			double ticksPerSecond;

			AnimInfo(double animDuration, double ticksPerSecond) 
				: 
				animDuration(animDuration), 
				ticksPerSecond(!ticksPerSecond ? defaultTicksPerSecond : ticksPerSecond) {}
		};

		struct AnimKeys
		{
			std::vector<std::pair<double, glm::vec3>> positionKeys;
			std::vector<std::pair<double, glm::quat>> rotationKeys;
			std::vector<std::pair<double, glm::vec3>> scalingKeys;

			bool KeysExist()
			{
				return !(positionKeys.empty() && rotationKeys.empty() && scalingKeys.empty());
			}
		};

		using BoneTree = TreeManager<std::string, BoneInfo>;
		using AnimKeyMap = std::unordered_map<std::string, std::vector<AnimKeys>>;
		using AnimInfoVect = std::vector<AnimInfo>;

		std::vector<MeshInfo> meshes;
		bool render = true;
		bool isDynamic = false;
		std::string shaderProgram = "0";
		std::function<void()> behaviour = nullptr;
		BoneTree boneTree;
		AnimKeyMap animKeyMap;
		AnimInfoVect animInfoVect;
		glm::mat4 globalInverseTransform = glm::mat4(1.0f);

		void AddMesh(const Mesh& mesh)
		{
			meshes.emplace_back(MeshInfo(mesh, render, isDynamic, shaderProgram, behaviour));
		}

		void ResetDefaults()
		{
			for (auto& mesh : meshes)
			{
				mesh.render.ResetBackup(&render);
				mesh.isDynamic.ResetBackup(&isDynamic);
				mesh.shaderProgram.ResetBackup(&shaderProgram);
				mesh.behaviour.ResetBackup(&behaviour);
			}
		}

		void NormalizeAllEmptyWeights()
		{
			for (auto& mesh : meshes)
			{
				for (auto& vert : mesh.mesh.vert)
				{
					vert.NormalizeIfEmptyWeights();
				}
			}
		}

		ModelInfo& operator=(const ModelInfo& modelInfo)
		{
			meshes = modelInfo.meshes;
			render = modelInfo.render;
			isDynamic = modelInfo.isDynamic;
			shaderProgram = modelInfo.shaderProgram;
			behaviour = modelInfo.behaviour;
			boneTree = modelInfo.boneTree;

			ResetDefaults();

			return *this;
		}
	};
}