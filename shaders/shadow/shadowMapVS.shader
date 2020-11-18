in vec3 vPos;
in vec3 normal;
in vec3 tangent;
in vec3 bitangent;

uniform mat4 Model;
uniform mat4 View;
uniform mat4 Projection;

void main()
{
    gl_Position =  Projection * View * Model * vec4(vPos, 1.0);
}
