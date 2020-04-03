layout (vertices = 4) out;

layout(std430, binding = 4) buffer layoutOuterTess
{
  vec4 outerTessLevel[];
};

void main(void){
    if (gl_InvocationID == 0)
    {
        float innerTess = max(max(outerTessLevel[gl_PrimitiveID].x, outerTessLevel[gl_PrimitiveID].y),max(outerTessLevel[gl_PrimitiveID].z, outerTessLevel[gl_PrimitiveID].w));

        gl_TessLevelInner[0] = innerTess;
        gl_TessLevelInner[1] = innerTess;
        gl_TessLevelOuter[0] = outerTessLevel[gl_PrimitiveID].z;
        gl_TessLevelOuter[1] = outerTessLevel[gl_PrimitiveID].y;
        gl_TessLevelOuter[2] = outerTessLevel[gl_PrimitiveID].w;
        gl_TessLevelOuter[3] = outerTessLevel[gl_PrimitiveID].x;
    }
    gl_out[gl_InvocationID].gl_Position = gl_in[gl_InvocationID].gl_Position;
}
