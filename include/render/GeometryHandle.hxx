#ifndef GEOMETRYHANDLE_HXX
#define GEOMETRYHANDLE_HXX

#include "Mesh.hxx"
#include "GLAbstraction.hxx"

class GeometryHandle
{
public:
  
  GeometryHandle(rx::MeshPtr pMesh);
  ~GeometryHandle();
  
  ArrayBuffer mVertex;

  ArrayBuffer mNormal;

  ArrayBuffer mTangent;

  ArrayBuffer mBitangent;

  ArrayBuffer mUV;
  
  ArrayBuffer mIndex;
  
  unsigned int m_elementCount;
  
  bool mHasTangents;
  
  bool mHasBitangents;
};

using GeometryHandlePtr = std::shared_ptr<GeometryHandle>;

#endif
