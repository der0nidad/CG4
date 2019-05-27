#version 330 core
#extension GL_ARB_separate_shader_objects : enable
layout(location = 0) in vec3 aPos;

out vec3 TexCoords;

uniform mat4 projection;
uniform mat4 view;
//out gl_PerVertex { vec4 gl_Position; };
void main()
{
    mat4 view1 = mat4(mat3(view));
    TexCoords = aPos;
    gl_Position = projection * view1 * vec4(aPos, 1.0);
}  
 
