layout (quads , equal_spacing, ccw) in;
uniform mat4 Model;
uniform mat4 View;
uniform mat4 Projection;
uniform float size;
uniform sampler2D toto;

uniform vec3 translateOrigin;
uniform float scaleOrigin;

uniform float near;
uniform float far;


#define PI 3.14159265359

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

vec2 cartToSphere(in vec3 pos)
{
  return vec2(atan(pos.y / pos.x), acos(pos.z));
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

  vec3 normalSphere = normalize(p);
  vec2 sphCoord = cartToSphere(normalSphere);
  float teta = sphCoord.x / (PI/2.0);
  float phi = sphCoord.y /  PI;
  teta = teta * 2048;
  phi = phi * 4096;

  float wHeight = texture2D(toto, vec2(teta, phi)).r;
  gradient = vec2(wHeight, wHeight);

  vec3 positionOnQuad = normalSphere * size;
  positionOnQuad = (positionOnQuad - translateOrigin) * scaleOrigin;
  positionOnQuad = positionOnQuad + normalSphere * wHeight * 0.00001 * scaleOrigin;

  quadColor = colorQuad[gl_PrimitiveID];
  gl_Position = Projection * View * Model * vec4(positionOnQuad, 1);

  gl_Position.z = 2.0*log(gl_Position.w + 1)/log(far + 1) - 1;
  gl_Position.z *= gl_Position.w;
}
