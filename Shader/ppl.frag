uniform vec3 lightPos;

varying vec3 vertex;
varying vec3 normal;
varying vec3 color;

void main()
{
    vec3 N=normalize(normal);
    vec3 L=normalize(lightPos);
    float diffuse=max(0.0,dot(N,L));
    gl_FragColor=vec4(color*(diffuse+0.05),1.0);
}
