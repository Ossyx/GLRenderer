#include "SceneGraph.hxx"
#include "math.h"
#include "MathTools.hxx"

namespace rx
{

SgNode::SgNode():
mId(0),
mType(NodeType::Unknown)
{
}

SgNode::SgNode(NodeType pType):
mId(0),
mType(pType)
{
}

SgNode::~SgNode()
{
}

unsigned int SgNode::Id() const
{
  return mId;
}

void SgNode::SetId(unsigned int pId)
{
  mId = pId;
}

std::list<SgNode::NodePtr> SgNode::Childrens() const
{
  return mChildrens;
}

void SgNode::AppendChild(NodePtr pChild)
{
  mChildrens.push_back(pChild);
}

NodeType SgNode::Type() const
{
  return mType;
}

MatrixTransform::MatrixTransform():
  SgNode(NodeType::MatrixTransform)
{  
}

MatrixTransform::MatrixTransform(const glm::mat4& pTransform):
  SgNode(NodeType::MatrixTransform),
  mTransform(pTransform)
{
}

MatrixTransform::~MatrixTransform()
{
}

glm::mat4 MatrixTransform::Transform() const
{
  return mTransform;
}

void MatrixTransform::SetTransform(glm::mat4 pTransform)
{
  mTransform = pTransform;
}

PositionAttitudeTransform::PositionAttitudeTransform():
  SgNode(NodeType::PositionAttitude)
{
}

PositionAttitudeTransform::PositionAttitudeTransform(const glm::vec3& pPosition,
  const glm::quat& pAttitude):
  SgNode(NodeType::PositionAttitude),
  mPosition(pPosition),
  mAttitude(pAttitude)
{
}

PositionAttitudeTransform::~PositionAttitudeTransform()
{
}

glm::vec3 PositionAttitudeTransform::Position() const
{
  return mPosition;
}

glm::quat PositionAttitudeTransform::Attitude() const
{
  return mAttitude;
}

glm::vec3 PositionAttitudeTransform::ToEulerAngle() const
{
  //Compute euler angles from quaternion
  //return as vec3(x, y, z)
  // roll (x-axis rotation)
  return QuaternionToEulerAngler(mAttitude);
}

void PositionAttitudeTransform::SetPosition(const glm::vec3& pPosition)
{
  mPosition = pPosition;
}

void PositionAttitudeTransform::SetAttitude(const glm::quat pAttitude)
{
  mAttitude = pAttitude;
}

void PositionAttitudeTransform::SetAttitudeFromEulerAngle(float pRoll, float pPitch, float pYaw)
{
  mAttitude = EulerAngleToQuaternion(pRoll, pPitch, pYaw);
}

ObjectNode::ObjectNode():
  SgNode(NodeType::Object)
{
}

ObjectNode::ObjectNode(std::shared_ptr<rx::Model> pModelRef):
  SgNode(NodeType::Object),
  mModelRef(pModelRef)
{
}

ObjectNode::~ObjectNode()
{
}

ModelPtr ObjectNode::ModelRef()
{
  return mModelRef;
}

void ObjectNode::SetModelRef(ModelPtr pModelRef)
{
  mModelRef = pModelRef;
}

ShaderPtr ObjectNode::ShaderRef()
{
  return mShaderRef;
}

void ObjectNode::SetShaderRef(ShaderPtr pShaderRef)
{
  mShaderRef = pShaderRef;
}

MaterialShaderPtr ObjectNode::MaterialShaderRef()
{
  return mMaterialShaderRef;
}

void ObjectNode::SetMaterialShaderRef(MaterialShaderPtr pMaterialShaderRef)
{
  mMaterialShaderRef = pMaterialShaderRef;
}

EnvironmentMapNode::EnvironmentMapNode():
SgNode(NodeType::EnvironmentMap)
{
}

EnvironmentMapNode::EnvironmentMapNode(MaterialPtr pMaterial):
SgNode(NodeType::EnvironmentMap),
mCubeMapMaterial(pMaterial)
{
}

EnvironmentMapNode::~EnvironmentMapNode()
{
}

void EnvironmentMapNode::SetCubeMapMaterial(MaterialPtr pMaterial)
{
  mCubeMapMaterial = pMaterial;
}

MaterialPtr EnvironmentMapNode::GetCubeMapMaterial()
{
  return mCubeMapMaterial;
}

SceneGraph::SceneGraph()
{
}

SceneGraph::~SceneGraph()
{
}

void SceneGraph::SetRoot ( NodePtr pRoot )
{
  mRoot = pRoot;
}

SceneGraph::NodePtr SceneGraph::GetRoot()
{
  return mRoot;
}

SceneGraph::NodeHashMap& SceneGraph::GetNodes()
{
  return mNodes;
}

void SceneGraph::AssociateResources(const ResourcesHolder& pHolder)
{
  auto itNode = mNodes.begin();
  for(; itNode != mNodes.end(); ++itNode)
  {
    NodePtr node = (*itNode).second;
    if( node->Type().get() == NodeType::Object )
    {
      ObjectNodePtr objetNode = std::static_pointer_cast<ObjectNode>(node);
    }
  }
}

NodeType::NodeType():
mType(NodeType::Unknown)
{
}

NodeType::~NodeType()
{
}

NodeType::NodeType(const std::string& ptype)
{
  *this = NodeType::FromString(ptype);
}

NodeType::NodeType(NodeType::NType ptype):
mType(ptype)
{
}

NodeType NodeType::FromString(const std::string& ptype)
{
  if( ptype == "OBJECT")
  {
    return NodeType(NodeType::Object);
  }
  else if ( ptype == "MATRIXTRANSFORM" )
  {
    return NodeType(NodeType::MatrixTransform);
  }
  else if ( ptype == "POSITIONATTITUDE" )
  {
    return NodeType(NodeType::PositionAttitude);
  }
  else if ( ptype == "ENVIRONMENTMAP" )
  {
    return NodeType(NodeType::EnvironmentMap);
  }
  else
  {
    return NodeType(NodeType::Unknown);
  }
}

std::string NodeType::ToString() const
{
  switch(mType)
  {
    case NodeType::Object:
      return "OBJECT";
    case NodeType::MatrixTransform:
      return "MATRIXTRANSFORM";
    case NodeType::PositionAttitude:
      return "POSITIONATTITUDE";
    case NodeType::EnvironmentMap:
      return "ENVIRONMENTMAP";
    case NodeType::Unknown:
      return "UNKNOWN";
    default:
      return "UNKNOWN";
  }
}

NodeType::NType NodeType::get() const
{
  return mType;
}

}






