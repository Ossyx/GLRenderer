in vec3 ipos;
out vec4 frag_colour;

uniform sampler2D render_target_one;
uniform sampler2D render_target_two;
uniform sampler2D render_target_three;

//Global variable
uniform vec3 light;
uniform vec3 cameraPos;

vec3 unpackNormal(vec3 p_mapped)
{
  return vec3(p_mapped.x * 2.0 - 1.0, p_mapped.y * 2.0 - 1.0, p_mapped.z * 2.0 - 1.0);
}

void main()
{
  //vec2 uvcoord = (ipos.xy + vec2(1.0, 1.0)) / 2.0;
  frag_colour = vec4(0.0, 0.0, 0.0, 1.0);

  if(ipos.x < 0.0 && ipos.y > 0.0)
  {
    vec2 uvcoord = vec2(ipos.x + 1.0, ipos.y);
    vec4 albedo = texture2D(render_target_one, uvcoord);
    frag_colour =  albedo;
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
    vec4 specular = texture2D(render_target_three, uvcoord);
    frag_colour = specular;
  }

  if(ipos.x > 0.0 && ipos.y < 0.0)
  {
    vec2 uvcoord = vec2(ipos.x, ipos.y + 1.0);
    vec4 albedo = texture2D(render_target_one, uvcoord);
    vec3 normal = unpackNormal(texture2D(render_target_two, uvcoord).xyz);
    vec4 specular = texture2D(render_target_three, uvcoord);

    //Basic phong shading
    vec3 nlight = normalize(light);
    float value = max(dot(normal, nlight), 0.0);
    float ambient = 0.2;
    vec3 refl = 2.0 * dot(nlight, normal) * normal - nlight;
    vec4 final_color = ambient * albedo + value * albedo + specular * vec4(1.0,1.0,1.0,1.0);
    frag_colour = final_color;
  }
}
