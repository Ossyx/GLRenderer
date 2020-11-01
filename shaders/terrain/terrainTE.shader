layout (quads , equal_spacing, ccw) in;
uniform mat4 Model;
uniform mat4 View;
uniform mat4 Projection;
uniform float size;

uniform vec3 translateOrigin;
uniform float scaleOrigin;
uniform float coastKFactor;

uniform float near;
uniform float far;

uniform int noiseLOctave;
uniform int noiseMOctave;
uniform int noiseHOctave;

layout(std430, binding = 3) buffer layoutDataQuad
{
  //Center position + scale
  vec4 dataQuad[];
};

layout(std430, binding = 4) buffer layoutColorQuad
{
  vec4 colorQuad[];
};

out vec2 gradient;
out float elevation;
out vec4 quadColor;
out vec3 normal;
out vec3 position;
out vec2 patchCoord;
out vec3 tan1;
out vec3 tan2;

//#define Use_Perlin
//#define Use_Value
#define Use_Simplex

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

// ========= Noise ===========

float value_noise(vec3 p)
{
    vec3 pi = floor(p);
    vec3 pf = p - pi;

    vec3 w = pf * pf * (3.0 - 2.0 * pf);

    return 	mix(
        		mix(
        			mix(hash31(pi + vec3(0, 0, 0)), hash31(pi + vec3(1, 0, 0)), w.x),
        			mix(hash31(pi + vec3(0, 0, 1)), hash31(pi + vec3(1, 0, 1)), w.x),
                    w.z),
        		mix(
                    mix(hash31(pi + vec3(0, 1, 0)), hash31(pi + vec3(1, 1, 0)), w.x),
        			mix(hash31(pi + vec3(0, 1, 1)), hash31(pi + vec3(1, 1, 1)), w.x),
                    w.z),
        		w.y);
}

float perlin_noise(vec3 p)
{
    vec3 pi = floor(p);
    vec3 pf = p - pi;

    vec3 w = pf * pf * (3.0 - 2.0 * pf);

    return 	mix(
        		mix(
                	mix(dot(pf - vec3(0, 0, 0), hash33(pi + vec3(0, 0, 0))),
                        dot(pf - vec3(1, 0, 0), hash33(pi + vec3(1, 0, 0))),
                       	w.x),
                	mix(dot(pf - vec3(0, 0, 1), hash33(pi + vec3(0, 0, 1))),
                        dot(pf - vec3(1, 0, 1), hash33(pi + vec3(1, 0, 1))),
                       	w.x),
                	w.z),
        		mix(
                    mix(dot(pf - vec3(0, 1, 0), hash33(pi + vec3(0, 1, 0))),
                        dot(pf - vec3(1, 1, 0), hash33(pi + vec3(1, 1, 0))),
                       	w.x),
                   	mix(dot(pf - vec3(0, 1, 1), hash33(pi + vec3(0, 1, 1))),
                        dot(pf - vec3(1, 1, 1), hash33(pi + vec3(1, 1, 1))),
                       	w.x),
                	w.z),
    			w.y);
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
#ifdef Use_Perlin
    return perlin_noise(p * 2.0);
#elif defined Use_Value
    return value_noise(p * 2.0);
#elif defined Use_Simplex
    return simplex_noise(p);
#endif

    return 0.0;
}

// ========== Different function ==========

float noise_itself(vec3 p)
{
    return noise(p * 8.0);
}

float noise_sum(vec3 p)
{
    float f = 0.0;
    p = p * 256.0;
    float s = 0.001;
    for(int i=0; i < 25; ++i)
    {
      f += s * noise(p); p = 2.0 * p;
      s = s / 2.0;
    }

    /*f += 1.0000 * noise(p); p = 2.0 * p;
    f += 0.5000 * noise(p); p = 2.0 * p;
	f += 0.2500 * noise(p); p = 2.0 * p;
	f += 0.1250 * noise(p); p = 2.0 * p;
	f += 0.0625 * noise(p); p = 2.0 * p;
	f += 0.0312 * noise(p); p = 2.0 * p;
	f += 0.0156 * noise(p); p = 2.0 * p;
	f += 0.0075 * noise(p); p = 2.0 * p;*/

    return f;
}

float noise_sum_H(vec3 p)
{
    float f = 0.0;
    p = p * 256.0;
    float s = 1.0;
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

float noise_sum_abs(vec3 p)
{
    float f = 0.0;
    p = p * 3.0;
    f += 1.0000 * abs(noise(p)); p = 2.0 * p;
    f += 0.5000 * abs(noise(p)); p = 2.0 * p;
	f += 0.2500 * abs(noise(p)); p = 2.0 * p;
	f += 0.1250 * abs(noise(p)); p = 2.0 * p;
	f += 0.0625 * abs(noise(p)); p = 2.0 * p;

    return f;
}

float noise_sum_abs_sin(vec3 p)
{
    float f = noise_sum_abs(p);
    f = sin(f * 2.5 + p.x * 5.0 - 1.5);

    return f ;
}

vec2 cartToSphere(in vec3 pos)
{
  return vec2(atan(pos.y / pos.x), acos(pos.z));
}

vec3 sphereToCart(vec2 coord)
{
  return vec3(sin(coord.y) * cos(coord.x), sin(coord.y) * sin(coord.x), cos(coord.y));
}

float calcElevation(vec3 pos)
{
  float e1 = noise_sum_L(pos) * 0.008;
  float e2 = (1.0 - abs(noise_sum_M(pos))) * 0.008;

  float value = e1;

  float coastMod = 1.0f / (1.0f + exp(-coastKFactor*(value-0.001)));
  value = coastMod * value;
  return value * scaleOrigin;
}

vec2 calcGrad( in vec3 pos, float elev)
{
    const float eps = 0.001;
    vec2 spCoord = cartToSphere(pos);

    vec3 v1 = sphereToCart(spCoord + vec2(1.0,0.0) *eps);
    vec3 v3 = sphereToCart(spCoord + vec2(0.0,1.0) *eps);

    return vec2(calcElevation(v1) - elev,
                calcElevation(v3) - elev);
}

void main(void)
{
  // interpolate in horizontal direction between vert. 0 and 3
  vec3 p0 = mix(gl_in[1].gl_Position.xyz, gl_in[2].gl_Position.xyz, gl_TessCoord.x);
  // interpolate in horizontal direction between vert. 1 and 2
  vec3 p1 = mix(gl_in[0].gl_Position.xyz, gl_in[3].gl_Position.xyz, gl_TessCoord.x);
  // interpolate in vert direction
  vec3 p = mix(p0, p1, gl_TessCoord.y);
  vec2 tePatchDistance = gl_TessCoord.xy;

  tan1 = normalize(gl_in[3].gl_Position.xyz - gl_in[0].gl_Position.xyz);
  tan2 = normalize(gl_in[1].gl_Position.xyz - gl_in[0].gl_Position.xyz);

  patchCoord = gl_TessCoord.xy;

  vec3 normalSphere = normalize(p);
  vec3 positionOnQuad = normalSphere * size;

  //Displacement
  elevation = calcElevation(positionOnQuad);
  position = positionOnQuad;

  quadColor = colorQuad[gl_PrimitiveID];
  positionOnQuad = (positionOnQuad - translateOrigin) * scaleOrigin;
  positionOnQuad = positionOnQuad  + normalSphere * elevation;

  gl_Position = Projection * View * Model * vec4(positionOnQuad, 1);

  gl_Position.z = 2.0*log(gl_Position.w + 1)/log(far + 1) - 1;
  gl_Position.z *= gl_Position.w;
}
