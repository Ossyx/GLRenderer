#include "ResourcesLoader.hxx"
#include <iostream>

int main()
{
  rx::ResourcesLoader loader;
  rx::ResourcesHolder holder;
  loader.LoadDescription("/home/bertrand/Work/GLRenderer/test/data/resources_index.json", holder);
  loader.LoadResources(holder);

  return 0;
}
