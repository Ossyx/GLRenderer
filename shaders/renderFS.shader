in vec3 ipos;
out vec4 frag_colour;

uniform sampler2D render_target_one;
uniform sampler2D render_target_two;
uniform sampler2D render_target_three;
uniform sampler2D render_target_four;
uniform sampler2D render_target_five;

//Global variable
uniform vec3 light;
uniform vec3 cameraPos;
uniform mat4 shadowMapMatrix;
uniform mat4 invertProjMatrix;
uniform mat4 invertViewMatrix;
uniform mat4 biasMatrix;

vec3 wordlPosFromDepth(float depth, vec2 ptexCoord) {
    float z = depth * 2.0 - 1.0;

    vec4 clipSpacePosition = vec4(ptexCoord * 2.0 - 1.0, z, 1.0);
    vec4 viewSpacePosition = invertProjMatrix * clipSpacePosition;

    // Perspective division
    viewSpacePosition /= viewSpacePosition.w;

    vec4 worldSpacePosition = invertViewMatrix * viewSpacePosition;

    return worldSpacePosition.xyz;
}

vec3 unpackNormal(vec3 p_mapped)
{
  return vec3(p_mapped.x * 2.0 - 1.0, p_mapped.y * 2.0 - 1.0, p_mapped.z * 2.0 - 1.0);
}

void main()
{
  //vec2 uvcoord = (ipos.xy + vec2(1.0, 1.0)) / 2.0;
  frag_colour = vec4(0.0, 0.0, 0.0, 1.0);

  float zNearPersp = 0.5;
  float zFarPersp  = 3000.0;

  float zNearOrth= 0.5;
  float zFarOrth  = 3000.0;

  if(ipos.x < 0.0 && ipos.y > 0.0)
  {
    vec2 uvcoord = vec2(ipos.x + 1.0, ipos.y);
    vec4 albedo = texture2D(render_target_one, uvcoord);
    frag_colour = albedo;
  }

  if(ipos.x > 0.0 && ipos.y > 0.0)
  {
    vec2 uvcoord = vec2(ipos.x, ipos.y);
    vec4 normal = texture2D(render_target_two, uvcoord);
    frag_colour = normal;
  }

  if(ipos.x < 0.0 && ipos.y < 0.0)
  {
    vec2 uvcoord = vec2(ipos.x + 1.0, ipos.y + 1.0);
    /*vec4 specular = texture2D(render_target_three, uvcoord);
    frag_colour = specular;*/
    float depth = texture2D(render_target_four, uvcoord).x;
    float depthColor = (2.0 * zNearPersp) / (zFarPersp + zNearPersp - depth * (zFarPersp - zNearPersp));
    frag_colour = vec4(depthColor, depthColor, depthColor, 1.0);
  }

  if(ipos.x > 0.0 && ipos.y < 0.0)
  {
    vec2 uvcoord = vec2(ipos.x, ipos.y + 1.0);

    //Shadow
    float depth = texture2D(render_target_four, uvcoord).x;
    vec3 posWorld = wordlPosFromDepth(depth, uvcoord);
    vec4 worldInShadow = biasMatrix * shadowMapMatrix * vec4(posWorld, 1.0);
    float shadowMapProj = texture2D(render_target_five, worldInShadow.xy).x;
    float dist = worldInShadow.z - shadowMapProj;
    vec4 shadow = vec4(0.0, 0.0, 0.0, 1.0);
    if(dist < 0.001 && depth < 1.0)
    {
      shadow = vec4(1.0, 1.0, 1.0, 1.0);
    }
    else
    {
      shadow = vec4(0.0, 0.0, 0.0, 1.0);
    }

    vec4 albedo = texture2D(render_target_one, uvcoord);
    vec3 normal = unpackNormal(texture2D(render_target_two, uvcoord).xyz);
    vec4 specular = texture2D(render_target_three, uvcoord);

    //Basic phong shading
    vec3 viewVector = normalize(cameraPos - posWorld);
    vec3 nlight = normalize(light);
    float value = max(dot(normal, nlight), 0.0);
    vec4 ambient = vec4(0.2, 0.2, 0.2, 1.0);
    vec3 refl = 2.0 * dot(nlight, normal) * normal - nlight;
    float spec = pow(max(dot(viewVector, refl), 0.0), 4.0) * specular.x;
    vec4 final_color = ambient * albedo + (value * albedo + spec * vec4(1.0,1.0,1.0,1.0)) * specular.y * shadow;
    frag_colour = final_color;
  }
}
