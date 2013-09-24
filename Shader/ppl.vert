varying vec3 vertex;
varying vec3 normal;
varying vec3 color;

void main()
{
    vertex      = gl_Vertex.xyz;
    normal = gl_Normal.xyz;
    color = gl_Color.xyz;
    gl_Position = ftransform();
}
