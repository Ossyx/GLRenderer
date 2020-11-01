#ifndef TERRAINGENPARAMETERS_HXX
#define TERRAINGENPARAMETERS_HXX

struct TerrainGenParameters
{
  float m_planetRadius;
  float m_normalEpsilon;

  unsigned int m_lowNoiseOctave;
  unsigned int m_mediumNoiseOctave;
  unsigned int m_highNoiseOctave;

  float m_elevationFactor;
};

#endif
