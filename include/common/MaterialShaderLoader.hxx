#ifndef MATERIALSHADERLOADER_HXX
#define MATERIALSHADERLOADER_HXX

#include "MaterialShader.hxx"
#include <json/value.h>
#include <json/reader.h>

#include <filesystem>
#include "GLRendererExport.hxx"

class GLRenderer_EXPORT MaterialShaderLoader
{
public:
  MaterialShaderLoader();
  virtual ~MaterialShaderLoader();
  
  static MaterialShaderPtr Load(std::filesystem::path const& pFilepath);  
  static MaterialShaderPtr Load(Json::Value& pContent);
  
};
#endif
