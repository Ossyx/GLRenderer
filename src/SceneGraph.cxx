#include "SceneGraph.hxx"

SgNode::SgNode()
{
}

SgNode::~SgNode()
{
}

TransformNode::TransformNode()
{
}

TransformNode::~TransformNode()
{
}

ObjectNode::ObjectNode()
{
}

ObjectNode::~ObjectNode()
{
}

SceneGraph::SceneGraph()
{
}

SceneGraph::~SceneGraph()
{
}

void SceneGraph::SetRoot ( SgNode* pRoot )
{
  mRoot = pRoot;
}

SgNode * SceneGraph::GetRoot()
{
  return mRoot;
}







