#version 330 core
out vec4 FragColor;

in vec3 TexCoords;

uniform samplerCube skybox;
//uniform bool FogState;
void main()
{    
    float density = 0.03;
    const float LOG2 = 1.442695;
    float z = gl_FragCoord.z / gl_FragCoord.w;
    float fogFactor = exp2( -density * density * z * z * LOG2);
    fogFactor = clamp(fogFactor, 0.0, 1.0);
    vec4 fog_color = vec4(0.91,0.91,0.91,0);
    
    FragColor = texture(skybox, TexCoords);
    
/*    if(FogState){
        FragColor = mix(fog_color, FragColor, fogFactor);
    }
*/
} 
