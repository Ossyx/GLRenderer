#include "SceneGraph.hxx"
#include "SceneGraphLoader.hxx"

#include "ResourcesHolder.hxx"
#include "ResourcesLoader.hxx"

int main()
{
  rx::ResourcesLoader rLoader;
  rx::ResourcesHolder holder;
  rLoader.LoadDescription("/home/bertrand/Work/GLRenderer/test/data/resources_scenegraphtest.json", holder);
  rLoader.LoadResources(holder);

  rx::SceneGraph graph;
  rx::SceneGraphLoader loader;  
  loader.Load("/home/bertrand/Work/GLRenderer/test/data/scenegraph.json", graph, holder);
  loader.Serialize("/home/bertrand/Work/GLRenderer/build/sceneGraphOut.json", graph);
  return 0;
}
