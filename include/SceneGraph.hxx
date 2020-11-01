#ifndef SCENEGRAPH_HXX
#define SCENEGRAPH_HXX

#include <glm/gtc/type_ptr.hpp>
#include <list>

class SgNode
{
public:
  SgNode();
  ~SgNode();

  std::list<SgNode*> mChildren;
};

class TransformNode : public SgNode
{
public:
  TransformNode();
  ~TransformNode();

  glm::mat4 mTransform;
};

class ObjectNode : public SgNode
{
public:
  ObjectNode();
  ~ObjectNode();
};


class SceneGraph
{
    
public:
  
  SceneGraph();
  ~SceneGraph();
  
  SgNode* GetRoot();
  
  void SetRoot(SgNode* pRoot);
  
private:
    
  SgNode* mRoot;
};
#endif
