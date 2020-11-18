#ifndef SCENEGRAPH_LOADER
#define SCENEGRAPH_LOADER

#include <filesystem>
#include <jsoncpp/json/value.h>
#include <jsoncpp/json/reader.h>
#include <jsoncpp/json/writer.h>

#include "SceneGraph.hxx"

class SceneGraphLoader
{
public:
  SceneGraphLoader();
  ~SceneGraphLoader();
  
  void Load(std::filesystem::path const& pSceneGraphFile, SceneGraph& pSceneGraph);
  void Load(Json::Value& pJsonDescription, SceneGraph& pSceneGraph);
  void Serialize(std::filesystem::path const& pSceneGraphFile, SceneGraph& pSceneGraph);
  void Serialize(Json::Value& pJsonDescription, SceneGraph& pSceneGraph);
  
private:
  
  SceneGraph::ObjectNodePtr LoadObjectNode(Json::Value& pDescription);
  SceneGraph::MatrixTransformPtr LoadMatrixTransformNode(Json::Value& pDescription);
  SceneGraph::PositionAttitudeTransformPtr LoadPositionAttitudeNode(Json::Value& pDescription);
  
  void SerializeObjectNode(SceneGraph::ObjectNodePtr pNode,
    Json::Value& pDescription);
  void SerializeMatrixTransformNode(SceneGraph::MatrixTransformPtr pNode,
    Json::Value& pDescription);
  void SerializePositionAttitudeNode(SceneGraph::PositionAttitudeTransformPtr pNode,
    Json::Value& pDescription);
  
  glm::mat4 ReadAsMat4(Json::Value& pData);
  glm::vec3 ReadAsVec3(Json::Value& pData);
  glm::vec4 ReadAsVec4(Json::Value& pData);
  glm::quat ReadAsQuat(Json::Value& pData);
  
  Json::Value Serialize(glm::vec3 const& pData);
  Json::Value Serialize(glm::vec4 const& pData);
  Json::Value Serialize(glm::mat4 const& pData);
  Json::Value Serialize(glm::quat const& pData);
  Json::Value Serialize(std::vector<int> const& pData);
  
  std::vector<unsigned int> ReadAsUIntArray(Json::Value& pData);
};
#endif
