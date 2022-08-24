#include "MaterialShaderLoader.hxx"
#include <fstream>

MaterialShaderLoader::MaterialShaderLoader()
{

}
MaterialShaderLoader::~MaterialShaderLoader()
{

}  

MaterialShaderPtr MaterialShaderLoader::Load(std::filesystem::path const& pFilepath)
{
  std::ifstream inputStreamJsonMap;
  inputStreamJsonMap.open(pFilepath);
  
  if( inputStreamJsonMap.is_open() == false)
  {
    rxLogError( "Cannot find resources index file " << pFilepath );
    assert(false);
  }

  Json::Value root;   // will contains the root value after parsing.
  Json::Reader reader;
  bool parsingSuccessful = reader.parse(inputStreamJsonMap, root );
  if (parsingSuccessful == false)
  {
      // report to the user the failure and their locations in the document.
    rxLogError("Fail to parse resources index file : "<<reader.getFormattedErrorMessages());
    assert(false);
  }
  
  return Load(root);
}

MaterialShaderPtr MaterialShaderLoader::Load(Json::Value& pContent)
{
  MaterialShaderPtr matShader = std::make_shared<MaterialShader>();
  
  std::string name = pContent["name"].asString();
  matShader->SetName(name);
  
  Json::Value& uniformSources = pContent["UniformSources"];
  for( auto uniform : uniformSources.getMemberNames() )
  {
    auto source = uniformSources[uniform].asString();
    matShader->SetUniformSource(uniform, source);
    rxLogInfo("Loading "<<uniform << " " << source);
  }
  
  Json::Value& defaultValues = pContent["DefaultValues"];
  for( auto uniform : defaultValues.getMemberNames() )
  {
    Json::Value& value = defaultValues[uniform];
    if( value.isDouble() )
    {
      rxLogInfo("Loading default for "<<uniform << " as float ");
      matShader->SetDefaultValue(uniform, value.asFloat());
    }
    else if ( value.isInt() )
    {
      rxLogInfo("Loading default for "<<uniform << " as int ");
      matShader->SetDefaultValue(uniform, value.asInt());
    }
    else if ( value.isArray() )
    {
      if ( value.size() == 2 )
      {
        rxLogInfo("Loading default for "<<uniform << " as vec2 ");
        glm::vec2 v(value[0].asFloat(), value[1].asFloat());
        matShader->SetDefaultValue(uniform, v);
      }
      else if ( value.size() == 3 )
      {
        rxLogInfo("Loading default for "<<uniform << " as vec3 ");
        glm::vec3 v(value[0].asFloat(), value[1].asFloat(), value[2].asFloat());
        matShader->SetDefaultValue(uniform, v);
      }
      else if ( value.size() == 4 )
      {
        rxLogInfo("Loading default for "<<uniform << " as vec4 ");
        glm::vec4 v(value[0].asFloat(), value[1].asFloat(), value[2].asFloat(), value[3].asFloat());
        matShader->SetDefaultValue(uniform, v);
      }
    }
  }
  
  return matShader;
}
