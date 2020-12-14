#include "ResourceDescription.hxx"
#include <sstream>

namespace rx
{
  
ResourceDescription::ResourceDescription():
mType(ResourceType::Unknown),
mData(Json::Value())
{
}

ResourceDescription::ResourceDescription(ResourceType pType, Json::Value const& pData):
mType(pType),
mData(pData)
{
}

std::string ResourceDescription::AsString() const
{
  std::stringstream out;
  Json::Value::const_iterator itResource = mData.begin();
  
  out << "["<< std::endl;
  for(; itResource != mData.end(); ++itResource)
  {
    Json::Value const& attr = *itResource;
    if( attr.isConvertibleTo(Json::stringValue) )
    {
      out << itResource.key().asString() << " : " << attr.asString() << std::endl;
    }
    else
    {
      out << itResource.key().asString() << " : [..] " << std::endl;
    }
  }
  out << "]"<< std::endl;
  
  return out.str();
}

ResourceDescription::~ResourceDescription()
{  
}

ResourceType::ResourceType():
mType(ResourceType::Unknown)
{
}

ResourceType::~ResourceType()
{
}

ResourceType::ResourceType(const std::string& ptype)
{
  *this = ResourceType::FromString(ptype);
}

ResourceType::ResourceType(ResourceType::RType ptype):
mType(ptype)
{
}

ResourceType ResourceType::FromString(const std::string& ptype)
{
  if( ptype == "MODEL")
  {
    return ResourceType(ResourceType::Model);
  }
  else if ( ptype == "MESH" )
  {
    return ResourceType(ResourceType::Mesh);
  }
  else if ( ptype == "TEXTURE" )
  {
    return ResourceType(ResourceType::Texture);
  }
  else if ( ptype == "SHADERSTACK" )
  {
    return ResourceType(ResourceType::ShaderStack);
  }
  else if ( ptype == "MATERIAL" )
  {
    return ResourceType(ResourceType::Material);
  }
  else if ( ptype == "MATERIALSHADER" )
  {
    return ResourceType(ResourceType::MaterialShader);
  }
  else
  {
    return ResourceType(ResourceType::Unknown);
  }
}

std::string ResourceType::ToString() const
{
  switch(mType)
  {
    case ResourceType::Model:
      return "MODEL";
    case ResourceType::Mesh:
      return "MESH";
    case ResourceType::Texture:
      return "TEXTURE";
    case ResourceType::ShaderStack:
      return "SHADERSTACK";
    case ResourceType::Material:
      return "MATERIAL";
    case ResourceType::MaterialShader:
      return "MATERIALSHADER";
    case ResourceType::Unknown:
      return "UNKNOWN";
    default:
      return "UNKNOWN";
  }
}

ResourceType::RType ResourceType::get() const
{
  return mType;
}

}
