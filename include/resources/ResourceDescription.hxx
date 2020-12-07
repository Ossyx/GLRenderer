#ifndef RESOURCEDESCRIPTION_HXX
#define RESOURCEDESCRIPTION_HXX

#include <string>
#include <jsoncpp/json/value.h>

namespace rx
{
  
class ResourceType
{
public:
  enum RType
  {
    Unknown,
    Texture,
    Mesh,
    Model,
    ShaderStack,
    Material
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


class ResourceDescription
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
