#include "SceneGraphLoader.hxx"
#include <fstream>
#include <glm/gtc/type_ptr.hpp>

namespace rx
{

SceneGraphLoader::SceneGraphLoader()
{
}

SceneGraphLoader::~SceneGraphLoader()
{
}

void SceneGraphLoader::Load(std::filesystem::path const& pSceneGraphFile, SceneGraph& pSceneGraph,
  ResourcesHolder& pHolder)
{
  std::ifstream inputStreamJsonMap;
  inputStreamJsonMap.open(pSceneGraphFile);
  
  if( inputStreamJsonMap.is_open() == false)
  {
    rxLogError( "Cannot find resources index file " << pSceneGraphFile );
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
  
  Load(root, pSceneGraph, pHolder);
}

void SceneGraphLoader::Load(Json::Value& pJsonDescription, SceneGraph& pSceneGraph, 
  ResourcesHolder& pHolder)
{
  Json::Value& nodes = pJsonDescription["nodes"];
  unsigned int rootId = pJsonDescription["root"].asInt();
  
  Json::Value::iterator itNodes = nodes.begin();
  
  SceneGraph::NodeHashMap& nodesStorage = pSceneGraph.GetNodes();
  
  for(; itNodes != nodes.end(); ++itNodes)
  {
    Json::Value& nodeData = *itNodes;
    NodeType type(nodeData["type"].asString());
    unsigned int id = nodeData["id"].asInt();
    
    SceneGraph::NodePtr nodeRef;
    
    switch(type.get())
    {
      case NodeType::Object:
        nodeRef = LoadObjectNode(nodeData, pHolder);
        break;
      case NodeType::MatrixTransform:
        nodeRef = LoadMatrixTransformNode(nodeData);
        break;
      case NodeType::PositionAttitude:
        nodeRef = LoadPositionAttitudeNode(nodeData);
        break;
      case NodeType::EnvironmentMap:
        nodeRef = LoadEnvironmentMapNode(nodeData, pHolder);
        break;
      case NodeType::Unknown:
        rxLogError("Unknown scene graph node type");
        assert(false);
        break;
    }
    
    nodeRef->SetId(id);
    nodesStorage[id] = nodeRef;
  }
  
  itNodes = nodes.begin();
  for(; itNodes != nodes.end(); ++itNodes)
  {
    Json::Value& childrenData = (*itNodes)["children"];
    Json::Value::iterator idChild = childrenData.begin();
    unsigned int id = (*itNodes)["id"].asInt();
    SceneGraph::NodePtr nodeRef = nodesStorage[id];
    for(; idChild != childrenData.end(); ++idChild)
    {
      nodeRef->AppendChild(nodesStorage[idChild->asInt()]);
    }
  }
  
  if( nodesStorage.find(rootId) != nodesStorage.end() )
  {
    pSceneGraph.SetRoot( nodesStorage[rootId] );
  }
  else
  {
    rxLogError("No node with id " << rootId << " for root.");
    assert(false);
  }
}

SceneGraph::ObjectNodePtr SceneGraphLoader::LoadObjectNode(Json::Value& pDescription, 
  ResourcesHolder& pHolder)
{
  rxLogInfo("Loading Object Node");
  SceneGraph::ObjectNodePtr node = std::make_shared<ObjectNode>();
  std::string modelRefName = pDescription["model"].asString();
  if( auto m = pHolder.FindModel( modelRefName ) )
  {
    node->SetModelRef( *m );
  }
  else
  {
    rxLogError( "No model named " << modelRefName );
  }
  
  std::string shaderRefName = pDescription["shader"].asString();
  if( auto s = pHolder.FindShader( shaderRefName ) )
  {
    node->SetShaderRef( *s );
  }
  else
  {
    rxLogError( "No shader named " << shaderRefName );
  }
  return node;
}

SceneGraph::MatrixTransformPtr
SceneGraphLoader::LoadMatrixTransformNode(Json::Value& pDescription)
{
  rxLogInfo("Loading Matrix Transform Node");
  SceneGraph::MatrixTransformPtr node = std::make_shared<MatrixTransform>(
    ReadAsMat4(pDescription["mat4"]));
  return node;
}

SceneGraph::PositionAttitudeTransformPtr 
SceneGraphLoader::LoadPositionAttitudeNode(Json::Value& pDescription)
{
  rxLogInfo("Loading Position Attitude Node");
  SceneGraph::PositionAttitudeTransformPtr node = 
    std::make_shared<PositionAttitudeTransform>(
    ReadAsVec3(pDescription["position"]),
    ReadAsQuat(pDescription["attitude"]));
  return node;
}

SceneGraph::EnvironmentMapPtr SceneGraphLoader::LoadEnvironmentMapNode(
  Json::Value& pDescription, ResourcesHolder& pHolder)
{
  rxLogInfo("Loading EnvironmentMap Node");
  SceneGraph::EnvironmentMapPtr node = std::make_shared<EnvironmentMapNode>();
  std::string materialRefName = pDescription["material"].asString();
  if( auto m = pHolder.FindMaterial( materialRefName ) )
  {
    node->SetCubeMapMaterial( *m );
  }
  else
  {
    rxLogError( "No material named " << materialRefName );
  }
  return node;
}

void SceneGraphLoader::Serialize(const std::filesystem::path& pSceneGraphFile, SceneGraph& pSceneGraph)
{
  std::ofstream out;
  out.open(pSceneGraphFile);
  
  if ( out.is_open() )
  {
    Json::StyledWriter styledWriter;
    Json::Value json;
    Serialize(json, pSceneGraph);
    out << styledWriter.write(json);
    out.close();
  }
  else
  {
    rxLogError( "Cannot create file " << pSceneGraphFile );
  }
}

void SceneGraphLoader::Serialize(Json::Value& pJsonDescription, SceneGraph& pSceneGraph)
{
  SceneGraph::NodeHashMap& nodesStorage = pSceneGraph.GetNodes();
  
  Json::Value nodesArray(Json::arrayValue);
  rxLogInfo("Got " << nodesStorage.size() << " nodes to serialize");
  auto itNodes = nodesStorage.begin();
  for(; itNodes != nodesStorage.end(); ++itNodes)
  {
    Json::Value nodeJson;
    SceneGraph::NodePtr node = itNodes->second;
    nodeJson["id"] = node->Id();
    nodeJson["type"] = Json::Value(node->Type().ToString());
    
    switch(node->Type().get())
    {
      case NodeType::Object:
        SerializeObjectNode(std::static_pointer_cast<ObjectNode>(node), nodeJson);
        break;
      case NodeType::MatrixTransform:
        SerializeMatrixTransformNode(
          std::static_pointer_cast<MatrixTransform>(node), nodeJson);
        break;
      case NodeType::PositionAttitude:
        SerializePositionAttitudeNode(
          std::static_pointer_cast<PositionAttitudeTransform>(node), nodeJson);
        break;
      case NodeType::EnvironmentMap:
        SerializeEnvironmentMapNode(
          std::static_pointer_cast<EnvironmentMapNode>(node), nodeJson);
        break;
      case NodeType::Unknown:
        rxLogError("Unknown scene graph node type");
        assert(false);
        break;
    }
    
    std::vector<unsigned int> childrenIds;
    auto childrens = node->Childrens();
    auto itChild = childrens.begin();
    
    Json::Value childrenIdArray(Json::arrayValue);
    
    for(; itChild != childrens.end(); ++itChild)
    {
      childrenIdArray.append((*itChild)->Id());
    }
    
    nodeJson["children"] = childrenIdArray;    
    nodesArray.append(nodeJson);
  }
  
  pJsonDescription["nodes"] = nodesArray;
}

void SceneGraphLoader::SerializeObjectNode(
  SceneGraph::ObjectNodePtr pNode, Json::Value& pDescription)
{
  pDescription["model"] = pNode->ModelRef()->GetName();
  pDescription["shader"] = pNode->ShaderRef()->GetName();
}

void SceneGraphLoader::SerializeMatrixTransformNode(
  SceneGraph::MatrixTransformPtr pNode, Json::Value& pDescription)
{
  pDescription["mat4"] = Serialize(pNode->Transform());
}

void SceneGraphLoader::SerializePositionAttitudeNode(
  SceneGraph::PositionAttitudeTransformPtr pNode, Json::Value& pDescription)
{
  pDescription["position"] = Serialize(pNode->Position());
  pDescription["attitude"] = Serialize(pNode->Attitude());
}

void SceneGraphLoader::SerializeEnvironmentMapNode(
  SceneGraph::EnvironmentMapPtr pNode, Json::Value& pDescription)
{
  pDescription["material"] = pNode->GetCubeMapMaterial()->GetName();
}

glm::mat4 SceneGraphLoader::ReadAsMat4(Json::Value& pData)
{
  assert(pData.isArray() && pData.size() == 16);
  float mat[16];
  for(int i=0; i < 16; ++i)
  {
    mat[i] = pData[i].asFloat();
  }
  return glm::make_mat4(mat);
}

glm::quat SceneGraphLoader::ReadAsQuat(Json::Value& pData)
{
  assert(pData.isArray() && pData.size() == 4);
  glm::quat q;
  q.w = pData[0].asFloat();
  q.x = pData[1].asFloat();
  q.y = pData[2].asFloat();
  q.z = pData[3].asFloat();
  return q;
}

glm::vec3 SceneGraphLoader::ReadAsVec3(Json::Value& pData)
{
  assert(pData.isArray() && pData.size() == 3);
  glm::vec3 pos;
  pos.x = pData[0].asFloat();
  pos.y = pData[1].asFloat();
  pos.z = pData[2].asFloat();
  return pos;
}

glm::vec4 SceneGraphLoader::ReadAsVec4(Json::Value& pData)
{
  assert(pData.isArray() && pData.size() == 4);
  glm::vec4 pos;
  pos.x = pData[0].asFloat();
  pos.y = pData[1].asFloat();
  pos.z = pData[2].asFloat();
  pos.w = pData[3].asFloat();
  return pos;
}

Json::Value SceneGraphLoader::Serialize(const glm::vec3& pData)
{
  Json::Value vec(Json::arrayValue);
  vec.append(pData.x);
  vec.append(pData.y);
  vec.append(pData.z);
  return vec;
}

Json::Value SceneGraphLoader::Serialize(const glm::vec4& pData)
{
  Json::Value vec(Json::arrayValue);
  vec.append(pData.x);
  vec.append(pData.y);
  vec.append(pData.z);
  vec.append(pData.w);
  return vec;
}

Json::Value SceneGraphLoader::Serialize(const glm::mat4& pData)
{
  Json::Value mat(Json::arrayValue);
  for(int i=0; i < 16; ++i)
  {
    mat.append(glm::value_ptr(pData)[i]);
  }
  return mat;
}

Json::Value SceneGraphLoader::Serialize(const glm::quat& pData)
{
  Json::Value q(Json::arrayValue);
  q.append(pData.w);
  q.append(pData.x);
  q.append(pData.y);
  q.append(pData.z);
  return q;
}

Json::Value SceneGraphLoader::Serialize(const std::vector<int>& pData)
{
  Json::Value vec(Json::arrayValue);
  for(int i=0; i < pData.size(); ++i)
  {
    vec.append(pData[i]);
  }
  return vec;
}

}





