#version 330 core
layout(location = 0) in vec3 vertex;
layout(location = 1) in vec3 normal;
layout(location = 2) in vec2 texCoords;


out vec2 vTexCoords;
out vec3 vFragPosition;
out vec3 vNormal;

uniform float time;
uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;
//uniform float str_const;

void main()
{
   
    vec3 vertex2 = vertex + vec3(0.0, 0.5 * sin(time + vertex.x *(128.0/127.0) *0.3141592653589793
 )  , 0.0f );
  gl_Position = projection * view * model * vec4(vertex2, 1.0f);

  vTexCoords = texCoords * 2;
  vFragPosition = vec3(model * vec4(vertex2, 1.0f));
  vNormal = mat3(transpose(inverse(model))) * normal;
}