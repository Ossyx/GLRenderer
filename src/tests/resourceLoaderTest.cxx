#include "ResourcesLoader.hxx"
#include <iostream>

int main(int argc, char** argv)
{
  if(argc >= 2)
  {
    std::string file(argv[1]);
    rxLogInfo( "Loading file "<< file );
    rx::ResourcesLoader loader;
    rx::ResourcesHolder holder;
    loader.LoadDescription(file, holder);
    loader.LoadResources(holder);
  }
  else
  {
    rxLogError( "No resources file provided." );
  }

  return 0;
}
