#include "RendererFactory.hxx"
#include "SimpleRenderer.hxx"
#include "DeferredRenderer.hxx"

RendererFactory::RendererPtr RendererFactory::Create(std::string const& pType)
{
  if( pType == "SimpleRenderer" )
  {
    return std::make_shared<SimpleRenderer>();
  }
  else if( pType == "DeferredRenderer" )
  {
    return std::make_shared<DeferredRenderer>();
  }
  else
  {
    rxLogError("Unsupported renderer type.")
    assert(false);
  }
}
