#ifndef SCENEGRAPH_HXX
#define SCENEGRAPH_HXX

#include "Model.hxx"
#include "ResourcesHolder.hxx"

#include <glm/gtc/type_ptr.hpp>
#include <list>

namespace rx
{
  
class NodeType
{
public:
  enum NType
  {
    Object,
    MatrixTransform,
    PositionAttitude,
    EnvironmentMap,
    Unknown
  };
  
  NodeType();
  NodeType(NodeType::NType ptype);
  NodeType(std::string const& ptype);
  ~NodeType();
  
  static NodeType FromString(std::string const& ptype);
  std::string ToString() const;
  NType get() const;
  NType mType;
};

class SgNode
{
public:
  using NodePtr = std::shared_ptr<SgNode>;
  SgNode();
  SgNode(NodeType pType);
  ~SgNode();
  
  unsigned int Id() const;
  void SetId(unsigned int pId);
  std::list<NodePtr> Childrens() const;  
  void AppendChild(NodePtr pChild);
  NodeType Type() const;
  
protected:
  unsigned int mId;
  std::list<NodePtr> mChildrens;
  NodeType mType;
};

using NodePtr = std::shared_ptr<SgNode>;

class MatrixTransform : public SgNode
{
public:
  MatrixTransform();
  MatrixTransform(glm::mat4 const& pTransform);
  ~MatrixTransform();
  
  glm::mat4 Transform() const;
  void SetTransform(glm::mat4 pTransform);
private:

  glm::mat4 mTransform;
};

class PositionAttitudeTransform : public SgNode
{
public:
  PositionAttitudeTransform();
  PositionAttitudeTransform(glm::vec3 const& pPosition, glm::quat const& pAttitude);
  ~PositionAttitudeTransform();
  
  glm::vec3 Position() const;
  glm::quat Attitude() const;
  glm::vec3 ToEulerAngle() const;
  
  void SetPosition(glm::vec3 const& pPosition);
  void SetAttitude(glm::quat const pAttitude);  
  void SetAttitudeFromEulerAngle(float pRoll, float pPitch, float pYaw);  
  
private:

  glm::vec3 mPosition;
  glm::quat mAttitude;
};

class ObjectNode : public SgNode
{
public:
  ObjectNode();
  ObjectNode(ModelPtr pModelRef);
  ~ObjectNode();
  
  ModelPtr ModelRef();
  void SetModelRef(ModelPtr pModelRef);
  ShaderPtr ShaderRef();
  void SetShaderRef(ShaderPtr pShaderRef);
  
private:
  ModelPtr mModelRef;
  ShaderPtr mShaderRef;
};

class EnvironmentMapNode : public SgNode
{
public:
  EnvironmentMapNode();
  EnvironmentMapNode(MaterialPtr pMaterial);
  ~EnvironmentMapNode();
  
  void SetCubeMapMaterial(MaterialPtr pMaterial);
  MaterialPtr GetCubeMapMaterial();
  
private:
  MaterialPtr mCubeMapMaterial;
};


class SceneGraph
{
    
public:
  
  using NodePtr = std::shared_ptr<SgNode>;
  using ObjectNodePtr = std::shared_ptr<ObjectNode>;
  using PositionAttitudeTransformPtr = std::shared_ptr<PositionAttitudeTransform>;
  using MatrixTransformPtr = std::shared_ptr<MatrixTransform>;
  using EnvironmentMapPtr = std::shared_ptr<EnvironmentMapNode>;
  using NodeHashMap = std::unordered_map<unsigned int, NodePtr>;
  
  SceneGraph();
  ~SceneGraph();
  
  NodePtr GetRoot();  
  void SetRoot(NodePtr pRoot);
  
  NodeHashMap& GetNodes();
  
  void AssociateResources(ResourcesHolder const& pHolder);
  
private:
    
  NodeHashMap mNodes;
  NodePtr mRoot;
};

using SceneGraphPtr = std::shared_ptr<SceneGraph>;

}
#endif
