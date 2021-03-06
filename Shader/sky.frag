uniform sampler2D glow;
uniform sampler2D color;

uniform sampler2D stars;

uniform vec3 lightPos;

varying vec3 vertex;

#define PI 3.1415926535897932384626433832795

void main()
{
    vec3 V = normalize(vertex);
    vec3 L = normalize(lightPos);

    // Compute the proximity of this fragment to the sun.

    float vl = dot(V, L);

    // Look up the sky color and glow colors.

    vec4 Kc = texture2D(color, vec2((L.z + 1.0) / 2.0, V.z));
    vec4 Kg = texture2D(glow,  vec2((L.z + 1.0) / 2.0, vl));

    vec4 starColor = texture2D(stars, vec2((atan(V.x,V.y)+PI)/(2*PI),acos(V.z)/PI));
//vec4(vec2((atan(V.x,V.y)+PI)/(2*PI),acos(V.z)/PI),0,1);//
    

    // Combine the color and glow giving the pixel value.

    gl_FragColor = vec4((Kc.rgb + Kg.rgb * Kg.a / 2.0)*Kc.a+(1.0-Kc.a)*starColor, 1.0);

//  gl_FragColor = vec4(Kc.rgb + Kg.rgb * Kg.a / 2.0, Kc.a);
//gl_FragColor=starColor;

    // draw sun
    if (vl>0.995)
    {
	float k=(vl-0.995)/(0.005);
        gl_FragColor.xyz+=vec3(k,k,k);
    }
}
