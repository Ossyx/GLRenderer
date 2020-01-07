layout(location = 0) out vec4 outAlbedo;
layout(location = 1) out vec4 outNormal;
layout(location = 2) out vec4 outSpecular;
layout(location = 3) out float outDepth;

//Global variable
uniform vec3 light;
uniform vec3 cameraPos;
uniform vec3 diffuse_color;

in vec2 gradient;
in float elevation;

void main()
{
  vec4 albedo = vec4(elevation, elevation, elevation, 1.0);
  outAlbedo = albedo;
  outNormal = vec4(gradient.x, gradient.y, 0.0, 1.0);
  outSpecular = vec4(1.0, 1.0, 1.0, 1.0);
  outDepth = gl_FragCoord.z;
}
