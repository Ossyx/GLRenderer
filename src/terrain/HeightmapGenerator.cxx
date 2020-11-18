#include "HeightmapGenerator.hxx"
#include <iostream>
#include <CImg.h>
#include <libnoise/noise.h>

HeightmapGenerator::HeightmapGenerator():
m_width(0),
m_height(0),
m_heightmap(nullptr)
{
}

HeightmapGenerator::~HeightmapGenerator()
{
}

glm::vec3 HeightmapGenerator::fractV3(glm::vec3 const& p_v)
{
  return glm::vec3( p_v.x - floor(p_v.x), p_v.y - floor(p_v.y), p_v.z - floor(p_v.z));
}

glm::vec3 HeightmapGenerator::floorV3(glm::vec3 const& p_v)
{
  return glm::vec3( floor(p_v.x), floor(p_v.y), floor(p_v.z));
}

glm::vec3 HeightmapGenerator::yzx(glm::vec3 const& p_v)
{
  return glm::vec3( p_v.y, p_v.z, p_v.x);
}

glm::vec3 HeightmapGenerator::yxz(glm::vec3 const& p_v)
{
  return glm::vec3( p_v.y, p_v.x, p_v.z);
}

glm::vec3 HeightmapGenerator::zxy(glm::vec3 const& p_v)
{
  return glm::vec3( p_v.z, p_v.x, p_v.y);
}

// Grab from https://www.shadertoy.com/view/4djSRW
#define MOD3 glm::vec3(.1031,.11369,.13787)
//#define MOD3 glm::vec3(443.8975,397.2973, 491.1871)
float HeightmapGenerator::hash31(glm::vec3 p3)
{
	p3  = fractV3(p3 * MOD3);
    p3 += glm::dot(p3, yzx(p3) + 19.19f);
    float x = (p3.x + p3.y) * p3.z;
    return -1.0 + 2.0 * (x - floor(x));
}

glm::vec3 HeightmapGenerator::hash33(glm::vec3 p3)
{
	p3 = fractV3(p3 * MOD3);
    p3 += glm::dot(p3, yxz(p3) + 19.19f);
    return -1.0f + 2.0f * fractV3(glm::vec3((p3.x + p3.y)*p3.z, (p3.x+p3.z)*p3.y, (p3.y+p3.z)*p3.x));
}

// ========= Noise ===========
float HeightmapGenerator::simplex_noise(glm::vec3 p)
{
    const float K1 = 0.333333333f;
    const float K2 = 0.166666667f;

    glm::vec3 i = floor(p + (p.x + p.y + p.z) * K1);
    glm::vec3 d0 = p - (i - (i.x + i.y + i.z) * K2);

    glm::vec3 e = step(glm::vec3(0.0f), d0 - yzx(d0));
	glm::vec3 i1 = e * (1.0f - zxy(e));
	glm::vec3 i2 = 1.0f - zxy(e) * (1.0f - e);

    glm::vec3 d1 = d0 - (i1 - 1.0f * K2);
    glm::vec3 d2 = d0 - (i2 - 2.0f * K2);
    glm::vec3 d3 = d0 - (1.0f - 3.0f * K2);

    glm::vec4 h = max(0.6f - glm::vec4(glm::dot(d0, d0), glm::dot(d1, d1), glm::dot(d2, d2), glm::dot(d3, d3)), 0.0f);
    glm::vec4 n = h * h * h * h * glm::vec4(glm::dot(d0, hash33(i)), glm::dot(d1, hash33(i + i1)), glm::dot(d2, hash33(i + i2)), glm::dot(d3, hash33(i + 1.0f)));

    return glm::dot(glm::vec4(31.316), n);
}

float HeightmapGenerator::noise(glm::vec3 p)
{
    return simplex_noise(p);
}

// ========== Different function ==========
float HeightmapGenerator::noise_sum_H(glm::vec3 p)
{
    float f = 0.0;
    p = p * 256.0f;
    float s = 0.001;
    for(int i=0; i < 64; ++i)
    {
      f += s * noise(p); p = 2.0f * p;
      s = s / 2.0;
    }
    return f;
}

float HeightmapGenerator::noise_sum_M(glm::vec3 p)
{
    float f = 0.0f;
    p = p * 32.0f;
    float s = 1.0;
    for(int i=0; i < 8; ++i)
    {
      f += s * noise(p); p = 2.0f * p;
      s = s / 2.0;
    }
    return f;
}

float HeightmapGenerator::noise_sum_L(glm::vec3 p)
{
    float f = 0.0f;
    p = p * 2.0f;
    float s = 1.0f;
    for(int i=0; i < 8; ++i)
    {
      f += s * noise(p); p = 2.0f * p;
      s = s / 2.0;
    }
    return f;
}

void HeightmapGenerator::Generate()
{
  m_width = 1024;
  m_height = 1024;
  m_heightmap = new float[m_width*m_height];

  glm::vec3 v1(-0.5, -0.5, 0.5);
  glm::vec3 v2(0.5, -0.5, 0.5);
  glm::vec3 v3(-0.5, 0.5, 0.5);
  glm::vec3 v4(0.5, 0.5, 0.5);
  cimg_library::CImg<unsigned char> hm(m_width, m_height, 1, 1, 0);

  noise::module::Perlin perlinL;
  perlinL.SetFrequency(2.0f);
  perlinL.SetLacunarity(2.0f);
  perlinL.SetOctaveCount(8);

  noise::module::Perlin perlinM;
  perlinM.SetFrequency(8.0f);
  perlinM.SetLacunarity(2.0f);
  perlinM.SetOctaveCount(8);

  noise::module::Perlin perlinH;
  perlinH.SetFrequency(10.0f);
  perlinH.SetLacunarity(2.0f);
  perlinH.SetOctaveCount(8);

  noise::module::RidgedMulti ridged;
  ridged.SetFrequency(2.0f);
  ridged.SetOctaveCount(8);

  double max = 0.0f;
  double min = 0.0f;

  for(int i=0; i < m_width; ++i)
  {
    float u = float(i) / m_width;
    for(int j=0; j < m_height; ++j)
    {
      float v = float(j) / m_height;

      glm::vec3 p0 = glm::mix(v1, v2, u);
      // interpolate in horizontal direction between vert. 1 and 2
      glm::vec3 p1 = glm::mix(v3, v4, u);
      // interpolate in vert direction
      glm::vec3 p = mix(p0, p1, v);
      glm::vec3 pos = glm::normalize(p);

      /*float e1 = fabs(noise_sum_L(pos));
      float e2 = fabs(noise_sum_M(pos));
      float e3 = fabs(noise_sum_H(pos));
      m_heightmap[i] = e1 * 0.002f + e1 * e2 * 0.005f + e1 * e2 * e3 * 0.008f;
      value = (m_heightmap[i] / 0.008f) * 255;*/

      float e1 = ridged.GetValue(pos.x, pos.y, pos.z);
      float e2 = -fabs(perlinM.GetValue(pos.x, pos.y, pos.z));
      float e3 = fabs(perlinH.GetValue(pos.x, pos.y, pos.z));
      m_heightmap[i * m_width + j] = e1 + e1 * e2;
      max = std::fmax(m_heightmap[i * m_width + j], max);
      min = std::fmin(m_heightmap[i * m_width + j], min);
    }
    std::cout<< "At " << i << std::endl;
  }

  for(int i=0; i < m_width; ++i)
  {
    for(int j=0; j < m_height; ++j)
    {
      unsigned char& value = hm(i, j, 0);
      float v = m_heightmap[i * m_width + j];
      value = ((v - min) / (max - min)) * 255;
    }
  }

  hm.save("heightmap.png");

  delete[] m_heightmap;
}
