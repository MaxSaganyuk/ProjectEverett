#pragma once

#include "LGLStructs.h"

#include <map>
#include <vector>
#include <string>
#include <functional>
#include <mutex>

#include "interfaces/ISolidSim.h"

class aiScene;
class aiMesh;
class aiNode;

class FileLoader
{
	const aiScene* modelHandle;
	std::vector<std::string> extraTextureName;
	std::map<std::string, LGLStructs::Texture> texturesLoaded;
	std::string nameToSet;

	std::mutex scriptWrapperLock;

	void ProcessNode(const aiNode* nodeHandle, LGLStructs::ModelInfo& model);
	bool GetTextureFilenames(const std::string& path);
	LGLStructs::Mesh ProcessMesh(const aiMesh* meshHandle);
public:
	FileLoader();
	~FileLoader();

	bool LoadModel(const std::string& file, const std::string& name, LGLStructs::ModelInfo& model);
	bool LoadTexture(
		const std::string& file,
		LGLStructs::Texture& texture, 
		unsigned char* data = nullptr, 
		size_t dataSize = 0
	);

	void FreeTextureData();
	void FreeDllData();
	std::string GetCurrentDir();
	bool GetFilesInDir(std::vector<std::string>& files, const std::string& dir);

	bool GetScriptFuncFromDLL(
		std::weak_ptr<std::function<void(const std::string&, ISolidSim&)>>& scriptFuncWeakPtr,
		const std::string& dllPath, 
		const std::string& funcName
	);
	void UnloadScriptDLL(const std::string& dllPath);
	std::vector<std::string> GetLoadedScriptDlls();
};
