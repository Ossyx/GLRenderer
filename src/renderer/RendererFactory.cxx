#include "RendererFactory.hxx"
#include "SimpleRenderer.hxx"

RendererFactory::RendererPtr RendererFactory::Create(std::string const& pType)
{
  if( pType == "SimpleRenderer")
  {
    return std::make_shared<SimpleRenderer>();
  }
  else
  {
    rxLogError("Unsupported renderer type.")
    assert(false);
  }
}
