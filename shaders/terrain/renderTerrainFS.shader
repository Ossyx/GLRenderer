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
in vec4 quadColor;

void main()
{
  vec4 albedo = quadColor;
  outAlbedo = albedo;

   outNormal = vec4(0.0, gradient.y, 0.0, 1.0);
  /*if(gradient.y > 0.05)
  {
    outNormal = vec4(0.0, gradient.y, 0.0, 1.0);
  }
  else
  {
    outNormal = vec4(0.0,0.0,1.0,1.0);
  }*/
  outSpecular = vec4(1.0, 1.0, 1.0, 1.0);
  outDepth = gl_FragCoord.z;
}
