#ifndef MATERIALSHADERLOADER_HXX
#define MATERIALSHADERLOADER_HXX

#include "MaterialShader.hxx"
#include <jsoncpp/json/value.h>
#include <jsoncpp/json/reader.h>

#include <filesystem>

class MaterialShaderLoader
{
public:
  MaterialShaderLoader();
  virtual ~MaterialShaderLoader();
  
  static MaterialShaderPtr Load(std::filesystem::path const& pFilepath);  
  static MaterialShaderPtr Load(Json::Value& pContent);
  
};
#endif
