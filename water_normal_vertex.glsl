#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;

out VS_OUT {
    vec3 normal;
} vs_out;

uniform mat4 projection;
uniform mat4 view;
uniform mat4 model;
uniform float time;

void main()
{
  
    vec3 aPos2 = aPos + vec3(0.0, 0.5 * sin(time + aPos.x *(128.0/127.0) *0.3141592653589793
 )  , 0.0f );
    mat3 normalMatrix = mat3(transpose(inverse(view * model)));
    vs_out.normal = vec3(projection * vec4(normalMatrix * aNormal, 1.0));
    gl_Position = projection * view * model * vec4(aPos2, 1.0); 
    
}

