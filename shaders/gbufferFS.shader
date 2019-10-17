layout(location = 0) out vec4 outAlbedo;
layout(location = 1) out vec4 outNormal;
layout(location = 2) out vec4 outSpecular;
layout(location = 3) out float outDepth;
in vec3 inormal;
in vec3 itangent;
in vec3 ibitangent;

#ifdef HAS_UVCOORDS
in vec2 iuvcoords;
#endif

in vec3 ipos;

//Global variable
uniform vec3 light;
uniform vec3 cameraPos;

#ifdef ALBEDO_TEXTURE
uniform sampler2D map_diffuse;
#elif defined ALBEDO_COLOR
uniform vec3 diffuse_color;
#endif

#ifdef SPECULAR_TEXTURE
uniform sampler2D map_specular;
#elif defined SPECULAR_COLOR
uniform vec3 specular_color;
#endif

#ifdef NORMAL_TEXTURE
uniform sampler2D map_normal;
#endif

#ifdef AO_TEXTURE
uniform sampler2D map_ao;
#endif

#ifdef ROUGHNESS_TEXTURE
uniform sampler2D map_roughness;
#elif defined ROUGHNESS_COLOR
vec3 roughness_color;
#endif

vec3 unpackNormal(vec3 p_mapped)
{
  return vec3(p_mapped.x * 2.0 - 1.0, p_mapped.y * 2.0 - 1.0, p_mapped.z * 2.0 - 1.0);
}

vec3 packNormal(vec3 p_mapped)
{
  return vec3(p_mapped.x + 1.0 / 2.0, p_mapped.y + 1.0 / 2.0, p_mapped.z + 1.0 / 2.0);
}

void main()
{
  mat3 tbnMat = mat3(itangent, ibitangent, inormal);

#ifdef NORMAL_TEXTURE
  vec3 normalFromNmap = tbnMat * unpackNormal(texture2D(map_normal, iuvcoords).xyz);
  vec3 finalNormal = normalize(inormal + normalFromNmap);
#else
  vec3 finalNormal = inormal;
#endif

#ifdef ALBEDO_TEXTURE
  vec3 albedo = texture2D(map_diffuse, iuvcoords).xyz;
#elif defined ALBEDO_COLOR
  vec3 albedo = diffuse_color;
#endif

#ifdef SPECULAR_TEXTURE
  float specCoeff = texture2D(map_specular, iuvcoords).x;
#elif defined SPECULAR_COLOR
  float specCoeff = specular_color.x;
#endif

#ifdef AO_TEXTURE
  float ao = texture2D(map_ao, iuvcoords).x;
#else
  float ao = 1.0;
#endif

#ifdef ROUGHNESS_TEXTURE
  float roughness = texture2D(map_roughness, iuvcoords).x;
#else
  float roughness = 1.0;
#endif

  outAlbedo = vec4(albedo, 1.0);
  outNormal = vec4(packNormal(finalNormal), 1.0);
  outSpecular = vec4(specCoeff, ao, roughness, 1.0);
  outDepth = gl_FragCoord.z;
}
