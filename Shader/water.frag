uniform sampler2D glow;
uniform sampler2D color;

uniform sampler2D stars;


uniform sampler2D detailWaves;

uniform float time;

uniform vec3 lightPos;
uniform vec3 eyePos;

varying vec3 vertex;
varying vec3 normal;

#define PI 3.1415926535897932384626433832795

float Fresnel(float ndotl, float bias, float frespow)
{
float facing=1.0-ndotl;
return max(bias+(1.0-bias)*pow(facing,frespow),0.0);
}

void main()
{
    vec3 view=normalize(vertex-eyePos);
    vec3 detailN1 = texture2D(detailWaves,vertex.xy/12.0+time/10.0*vec2(0.4,-0.2)).xyz;
    vec3 detailN2 = texture2D(detailWaves,vertex.xy/15.0+time/12.0*vec2(0.1,0.3)).xyz;
    vec3 N=normal;
    N.xy+=abs(view.z)*(detailN1.xy+detailN2.xy);
    N = normalize(N);
    vec3 V = normalize(reflect(view,N));//normalize(vertex);
    vec3 L = normalize(lightPos);

    // Compute the proximity of this fragment to the sun.

    float vl = dot(V, L);

    // Look up the sky color and glow colors.

    vec4 Kc = texture2D(color, vec2((L.z + 1.0) / 2.0, V.z));
    vec4 Kg = texture2D(glow,  vec2((L.z + 1.0) / 2.0, vl));

    vec3 starColor = texture2D(stars, vec2((atan(V.x,V.y)+PI)/(2.0*PI),acos(V.z)/PI)).xyz;


    float fresnel=1.0-Fresnel(abs(view.z),0.1,5.0);
    vec3 waterColor=1.2*vec3(0,0.07,0.125)*(1.0-vl);

    // Combine the color and glow giving the pixel value.
    vec3 reflColor=(Kc.rgb + Kg.rgb * Kg.a / 2.0)*Kc.a+(1.0-Kc.a)*starColor;
    // draw sun
    if (vl>0.995)
    {
	float k=(vl-0.995)/(0.005);
        reflColor+=vec3(k,k,k);
    }

    float diffuse=max(0.0,dot(N,L));
    float specular=0.0;
    if (dot(N,L) > 0.0) 
       specular=pow(max(0.0, dot(reflect(-L, N), view)), 16.0);
    vec3 lighting=diffuse*0.3+specular;

    gl_FragColor = vec4((1.0-fresnel)*reflColor+fresnel*(abs(view.z)*waterColor+waterColor)+lighting, 1.0);

}
