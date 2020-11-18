#include "SceneGraph.hxx"
#include "SceneGraphLoader.hxx"

int main()
{
  SceneGraph graph;
  SceneGraphLoader loader;
  loader.Load("/home/bertrand/Work/GLRenderer/test/data/scenegraph.json", graph);  
  loader.Serialize("/home/bertrand/Work/GLRenderer/build/sceneGraphOut.json", graph);
  return 0;
}
