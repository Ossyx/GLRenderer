#include "GeometryHandle.hxx"

GeometryHandle::GeometryHandle(rx::MeshPtr pMesh):
m_elementCount(0),
mHasTangents(false),
mHasBitangents(false)
{
  rxLogInfo("PrepareBuffer with ");
  rxLogInfo("Triangle Count : "<< pMesh->GetTriangleCount());
  rxLogInfo("Vertex Count : "<< pMesh->GetVertexCount());
  
  mVertex.Build(GL_ARRAY_BUFFER, pMesh->GetVertexCount()*3*sizeof(float),
    pMesh->GetVertices());

  mNormal.Build(GL_ARRAY_BUFFER, pMesh->GetNormalCount()*3*sizeof(float),
    pMesh->GetNormals());
  
  mHasTangents = pMesh->HasTangents();

  if (mHasTangents)
  {
    mTangent.Build(GL_ARRAY_BUFFER, pMesh->GetTangentCount()*3*sizeof(float),
      pMesh->GetTangents());
  }

  mHasBitangents = pMesh->HasBitangents();
  if (mHasBitangents)
  {
    mBitangent.Build(GL_ARRAY_BUFFER, pMesh->GetBitangentCount()*3*sizeof(float),
      pMesh->GetBitangents());
  }

  mUV.Build(GL_ARRAY_BUFFER, pMesh->GetUVCoordsCount()*2*sizeof(float),
    pMesh->GetUVCoords());
  
  mIndex.Build(GL_ELEMENT_ARRAY_BUFFER, pMesh->GetTriangleCount()*3*sizeof(unsigned int),
    pMesh->GetTrianglesIndex());
  
  m_elementCount = pMesh->GetTriangleCount()*3;
}

GeometryHandle::~GeometryHandle()
{
}


