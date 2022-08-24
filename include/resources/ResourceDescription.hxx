#ifndef RESOURCEDESCRIPTION_HXX
#define RESOURCEDESCRIPTION_HXX

#include <string>
#include <json/value.h>
#include "GLRendererExport.hxx"

namespace rx
{
  
class GLRenderer_EXPORT ResourceType
{
public:
  enum RType
  {
    Unknown,
    Texture,
    Mesh,
    Model,
    ShaderStack,
    Material,
    MaterialShader
  };
  
  ResourceType();
  ResourceType(ResourceType::RType ptype);
  ResourceType(std::string const& ptype);
  ~ResourceType();
  
  static ResourceType FromString(std::string const& ptype);
  std::string ToString() const;
  RType get() const;
  RType mType;
};


class GLRenderer_EXPORT ResourceDescription
{
public:
  
  ResourceDescription();
  ResourceDescription(ResourceType pType, Json::Value const& pData);
  ~ResourceDescription();
  
  std::string AsString() const;
  
  ResourceType mType;
  Json::Value mData;
};

}

#endif
