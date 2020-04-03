in vec3 vPos;
in vec3 normal;
in vec3 tangent;
in vec3 bitangent;

#ifdef HAS_UVCOORDS
in vec2 uvcoords;
#endif

uniform mat4 Model;
uniform mat4 View;
uniform mat4 Projection;

out vec3 inormal;
out vec3 itangent;
out vec3 ibitangent;

#ifdef HAS_UVCOORDS
out vec2 iuvcoords;
#endif

#ifdef DISPLACEMENT_TEXTURE
uniform sampler2D map_displacement;
#endif

out vec3 ipos;

void main()
{
    vec4 projectedNormal = Model * vec4(normal, 0.0);
    inormal = normalize(projectedNormal.xyz);

    vec4 projectedTangent = Model * vec4(tangent, 0.0);
    itangent = normalize(projectedTangent.xyz);

    vec4 projectedBitangent = Model * vec4(bitangent, 0.0);
    ibitangent = normalize(projectedBitangent.xyz);

#ifdef HAS_UVCOORDS
    iuvcoords = uvcoords;
#endif

    ipos = vec4(Model * vec4(vPos, 1.0)).xyz;
    vec4 projectedVertex = Projection * View * Model * vec4(vPos, 1.0);

#ifdef DISPLACEMENT_TEXTURE
    float displacement = texture2D(map_displacement, iuvcoords).x;
    projectedVertex = projectedVertex + displacement * vec4(inormal, 1.0);
#endif
    gl_Position =  projectedVertex;
}
