layout (quads , equal_spacing, ccw) in;
uniform mat4 Model;
uniform mat4 View;
uniform mat4 Projection;

layout(std430, binding = 3) buffer layoutDataQuad
{
  //Center position + scale
  vec4 dataQuad[];
};

out vec2 gradient;
out float elevation;

//Noise functions by candycat from shadertoy https://www.shadertoy.com/view/ldc3RB
#define Use_Perlin
//#define Use_Value
//#define Use_Simplex

// ========= Noise ===========
vec2 hash22(vec2 p)
{
    p = vec2( dot(p,vec2(127.1,311.7)),
			  dot(p,vec2(269.5,183.3)));

    //return normalize(-1.0 + 2.0 * fract(sin(p)*43758.5453123));
    return -1.0 + 2.0 * fract(sin(p)*43758.5453123);
}

float hash21(vec2 p)
{
	float h = dot(p,vec2(127.1,311.7));

    return -1.0 + 2.0 * fract(sin(h)*43758.5453123);
}

float perlin_noise(vec2 p)
{
    vec2 pi = floor(p);
    vec2 pf = p - pi;

    vec2 w = pf * pf * (3.0 - 2.0 * pf);

    return mix(mix(dot(hash22(pi + vec2(0.0, 0.0)), pf - vec2(0.0, 0.0)),
                   dot(hash22(pi + vec2(1.0, 0.0)), pf - vec2(1.0, 0.0)), w.x),
               mix(dot(hash22(pi + vec2(0.0, 1.0)), pf - vec2(0.0, 1.0)),
                   dot(hash22(pi + vec2(1.0, 1.0)), pf - vec2(1.0, 1.0)), w.x),
               w.y);
}

float value_noise(vec2 p)
{
    vec2 pi = floor(p);
    vec2 pf = p - pi;

    vec2 w = pf * pf * (3.0 - 2.0 * pf);

    return mix(mix(hash21(pi + vec2(0.0, 0.0)), hash21(pi + vec2(1.0, 0.0)), w.x),
               mix(hash21(pi + vec2(0.0, 1.0)), hash21(pi + vec2(1.0, 1.0)), w.x),
               w.y);
}

float simplex_noise(vec2 p)
{
    const float K1 = 0.366025404; // (sqrt(3)-1)/2;
    const float K2 = 0.211324865; // (3-sqrt(3))/6;

    vec2 i = floor(p + (p.x + p.y) * K1);

    vec2 a = p - (i - (i.x + i.y) * K2);
    vec2 o = (a.x < a.y) ? vec2(0.0, 1.0) : vec2(1.0, 0.0);
    vec2 b = a - (o - K2);
    vec2 c = a - (1.0 - 2.0 * K2);

    vec3 h = max(0.5 - vec3(dot(a, a), dot(b, b), dot(c, c)), 0.0);
    vec3 n = h * h * h * h * vec3(dot(a, hash22(i)), dot(b, hash22(i + o)), dot(c, hash22(i + 1.0)));

    return dot(vec3(70.0, 70.0, 70.0), n);
}

float noise(vec2 p) {
#ifdef Use_Perlin
    return perlin_noise(p);
#elif defined Use_Value
    return value_noise(p);
#elif defined Use_Simplex
    return simplex_noise(p);
#endif

    return 0.0;
}

// ========== Different function ==========
float noise_itself(vec2 p)
{
    return noise(p * 8.0);
}

float noise_sum(vec2 p)
{
    float f = 0.0;
    p = p * 4.0;
    f += 1.0000 * noise(p); p = 2.0 * p;
    f += 0.5000 * noise(p); p = 2.0 * p;
	f += 0.2500 * noise(p); p = 2.0 * p;
	f += 0.1250 * noise(p); p = 2.0 * p;
	f += 0.0625 * noise(p); p = 2.0 * p;
	f += 0.0312 * noise(p); p = 2.0 * p;
	f += 0.0126 * noise(p); p = 2.0 * p;
	f += 0.0063 * noise(p); p = 2.0 * p;
	f += 0.0031 * noise(p); p = 2.0 * p;

    return f;
}

float noise_sum_abs(vec2 p)
{
    float f = 0.0;
    p = p * 7.0;
    f += 1.0000 * abs(noise(p)); p = 2.0 * p;
    f += 0.5000 * abs(noise(p)); p = 2.0 * p;
	f += 0.2500 * abs(noise(p)); p = 2.0 * p;
	f += 0.1250 * abs(noise(p)); p = 2.0 * p;
	f += 0.0625 * abs(noise(p)); p = 2.0 * p;

    return f;
}

float noise_sum_abs_sin(vec2 p)
{
    float f = noise_sum_abs(p);
    f = sin(f * 1.5 + p.x * 7.0);

    return f * f;
}

vec2 calcGrad( in vec2 pos )
{
    const float eps = 0.002;
    const vec2 v1 = vec2(1.0,0.0);
    const vec2 v2 = vec2(-1.0,0.0);
    const vec2 v3 = vec2(0.0,1.0);
    const vec2 v4 = vec2(0.0,-1.0);

    return vec2(noise_sum( pos + v1*eps )*0.1 - noise_sum( pos + v2*eps )*0.1,
                noise_sum( pos + v3*eps )*0.1 - noise_sum( pos + v4*eps )*0.1);
}


void main(void)
{
  /*vec4 position =   gl_TessCoord.x * gl_in[0].gl_Position +
                    gl_TessCoord.y * gl_in[1].gl_Position +
                    gl_TessCoord.z * gl_in[2].gl_Position;
  gl_Position = Projection * View * Model * position;*/

  // interpolate in horizontal direction between vert. 0 and 3
  vec3 p0 = mix(gl_in[1].gl_Position.xyz, gl_in[2].gl_Position.xyz, gl_TessCoord.x);
  // interpolate in horizontal direction between vert. 1 and 2
  vec3 p1 = mix(gl_in[0].gl_Position.xyz, gl_in[3].gl_Position.xyz, gl_TessCoord.x);
  // interpolate in vert direction
  vec3 p = mix(p0, p1, gl_TessCoord.y);
  vec2 tePatchDistance = gl_TessCoord.xy;
  vec3 positionOnQuad = p * dataQuad[gl_PrimitiveID].w  + dataQuad[gl_PrimitiveID].xyz;

  //Displacement
  elevation = noise_sum(positionOnQuad.xz);
  positionOnQuad.y = elevation*0.1;
  gradient = normalize(calcGrad(positionOnQuad.xz));

  gl_Position = Projection * View * Model * vec4(positionOnQuad, 1);
}
