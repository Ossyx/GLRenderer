layout(location = 0) out vec4 outAlbedo;
layout(location = 1) out vec4 outNormal;
layout(location = 2) out vec4 outSpecular;
layout(location = 3) out float outDepth;

//Global variable
uniform vec3 light;
uniform vec3 cameraPos;
uniform vec3 diffuse_color;

uniform float size;
uniform float surface_distance;
uniform float coastKFactor;

in vec2 gradient;
in float elevation;
in vec4 quadColor;
in vec3 normal;
in vec3 position;
in vec2 patchCoord;
in vec3 tan1;
in vec3 tan2;

uniform int noiseLOctave;
uniform int noiseMOctave;
uniform int noiseHOctave;

// ========= Hash ===========

vec3 hashOld33(vec3 p)
{
	p = vec3( dot(p,vec3(127.1,311.7, 74.7)),
			  dot(p,vec3(269.5,183.3,246.1)),
			  dot(p,vec3(113.5,271.9,124.6)));

    return -1.0 + 2.0 * fract(sin(p)*43758.5453123);
}

float hashOld31(vec3 p)
{
    float h = dot(p,vec3(127.1,311.7, 74.7));

    return -1.0 + 2.0 * fract(sin(h)*43758.5453123);
}

// Grab from https://www.shadertoy.com/view/4djSRW
#define MOD3 vec3(.1031,.11369,.13787)
//#define MOD3 vec3(443.8975,397.2973, 491.1871)
float hash31(vec3 p3)
{
	p3  = fract(p3 * MOD3);
    p3 += dot(p3, p3.yzx + 19.19);
    return -1.0 + 2.0 * fract((p3.x + p3.y) * p3.z);
}

vec3 hash33(vec3 p3)
{
	p3 = fract(p3 * MOD3);
    p3 += dot(p3, p3.yxz+19.19);
    return -1.0 + 2.0 * fract(vec3((p3.x + p3.y)*p3.z, (p3.x+p3.z)*p3.y, (p3.y+p3.z)*p3.x));
}

float simplex_noise(vec3 p)
{
    const float K1 = 0.333333333;
    const float K2 = 0.166666667;

    vec3 i = floor(p + (p.x + p.y + p.z) * K1);
    vec3 d0 = p - (i - (i.x + i.y + i.z) * K2);

    // thx nikita: https://www.shadertoy.com/view/XsX3zB
    vec3 e = step(vec3(0.0), d0 - d0.yzx);
	vec3 i1 = e * (1.0 - e.zxy);
	vec3 i2 = 1.0 - e.zxy * (1.0 - e);

    vec3 d1 = d0 - (i1 - 1.0 * K2);
    vec3 d2 = d0 - (i2 - 2.0 * K2);
    vec3 d3 = d0 - (1.0 - 3.0 * K2);

    vec4 h = max(0.6 - vec4(dot(d0, d0), dot(d1, d1), dot(d2, d2), dot(d3, d3)), 0.0);
    vec4 n = h * h * h * h * vec4(dot(d0, hash33(i)), dot(d1, hash33(i + i1)), dot(d2, hash33(i + i2)), dot(d3, hash33(i + 1.0)));

    return dot(vec4(31.316), n);
}

float noise(vec3 p) {
    return simplex_noise(p);
}

float noise_sum_H(vec3 p)
{
    float f = 0.0;
    p = p * 256.0;
    float s = 0.001;
    for(int i=0; i < noiseHOctave; ++i)
    {
      f += s * noise(p); p = 2.0 * p;
      s = s / 2.0;
    }
    return f;
}

float noise_sum_M(vec3 p)
{
    float f = 0.0;
    p = p * 16.0;
    float s = 1.0;
    for(int i=0; i < noiseMOctave; ++i)
    {
      f += s * abs(noise(p)); p = 2.0 * p;
      s = s / 2.0;
    }
    return f;
}

float noise_sum_L(vec3 p)
{
    float f = 0.0;
    p = p * 0.2;
    float s = 1.0;
    for(int i=0; i < noiseLOctave; ++i)
    {
      f += s * noise(p); p = 2.0 * p;
      s = s / 2.0;
    }
    return f;
}

vec2 cartToSphere(in vec3 pos)
{
  return vec2(atan(pos.y, pos.x), atan(sqrt(pos.x*pos.x+pos.y*pos.y),pos.z));
}

vec3 sphereToCart(vec2 coord)
{
  return vec3(sin(coord.y) * cos(coord.x), sin(coord.y) * sin(coord.x), cos(coord.y));
}

float calcElevation(vec3 pos)
{
  float e1 = noise_sum_L(pos) * 0.008;
  float coastMod = 1.0f / (1.0f + exp(-coastKFactor*(e1-0.001)));
  e1 = coastMod * e1;
  return e1;
}

vec3 calcNormal(vec3 posReal, vec3 posNorm)
{
    float d = length(cameraPos - posReal);
    const float eps = max((d - 0.01), 0.1) / 2307.0;
    //const float eps = 0.00005;
    vec2 spCoord = cartToSphere(posNorm);

    //vec3 v1 = sphereToCart(spCoord + vec2(1.0,0.0) *eps);
    //vec3 v3 = sphereToCart(spCoord + vec2(0.0,1.0) *eps);
    vec3 v1 = normalize(posNorm + tan1 * eps);
    vec3 v3 = normalize(posNorm + tan2 * eps);

    float elevV1 = calcElevation(v1*size);
    float elevV3 = calcElevation(v3*size);

    vec3 p1 = v1 * size + v1 * elevV1;
    vec3 p3 = v3 * size + v3 * elevV3;
    vec3 n = normalize(cross(p1 - posReal, p3 - posReal));
    if(dot(n, posNorm) > 0)
    {
      return n;
    }
    else
    {
      return -n;
    }
}

vec3 packNormal(vec3 p_mapped)
{
  return vec3(p_mapped.x + 1.0 / 2.0, p_mapped.y + 1.0 / 2.0, p_mapped.z + 1.0 / 2.0);
}

void main()
{
  vec3 renormPosition = normalize(position);
  float interElev = calcElevation(renormPosition * size);
  vec3 interPos = renormPosition * size + renormPosition * interElev;
  vec3 pNormal = packNormal(calcNormal(interPos, renormPosition));
  outAlbedo = quadColor;
  outNormal = vec4(pNormal, 1.0);
  outSpecular = vec4(patchCoord.x, patchCoord.y, 0.0, 1.0);
  outDepth = gl_FragCoord.z;
}
