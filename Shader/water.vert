varying vec3 vertex;
varying vec3 normal;

void main()
{
    vertex      = gl_Vertex.xyz;
    normal      = gl_Normal.xyz;
    gl_Position = ftransform();
}
