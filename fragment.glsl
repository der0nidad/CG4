#version 330 core
in vec3 vFragPosition;
in vec2 vTexCoords;
in vec3 vNormal;

out vec4 color;
uniform sampler2D ourTexture1;
uniform sampler2D textureVoter;
uniform float time;
uniform int normalized;
uniform bool FogState;
uniform bool waterState;

void main()
{
  int normalview = normalized;
  float density = 0.035;
  const float LOG2 = 1.442695;
  float z = gl_FragCoord.z / gl_FragCoord.w;
  float fogFactor = exp2( -density * density * z * z * LOG2);
  fogFactor = clamp(fogFactor, 0.0, 1.0);
  vec4 fog_color = vec4(0.81,0.81,0.81,0);

  //vec3 lightcolor = vec3(1.0f, 1.0f, 1.0f) * sin(time);
  vec3 lightcolor = vec3(1.0f, 1.0f, 1.0f);
  vec3 normal = vNormal;
  if(!gl_FrontFacing)
    normal = (-1.0f) * normal;

  //normal = (min(sin(time*0.9) + 0.2, 1.0))* normal;
  vec3 lightDir = vec3(1.0f, 1.0f, 0.0f); 
// Ambient
    float ambientStrength = 0.2f;
    vec3 ambient = ambientStrength * lightcolor;//color of light here

  float kd = max(dot(normal, lightDir), 0.0);

  //color = vec4(mix(vec4((ambient + kd) * col, 1.0f), texture(ourTexture1, vTexCoords), 0.5));
    color = vec4((ambient + kd) * texture(ourTexture1, vTexCoords).rgb, 1.0f);
    //color = vec4(vNormal,1.0f).rbga;//normal map
  if(vFragPosition.y > 6.5)
  {
     color = vec4((ambient + kd) * vec3(1.0f,1.0f,1.0f), 1.0f);
     //color = vec4(vec3(1.0f,1.0f,1.0f), 1.0f);
  }

  if((!waterState) && (vFragPosition.y < 2))
  {
    //color = vec4((ambient + kd) * vec3(0.130f,0.6f,0.80f), 1.0f);

    color = vec4((ambient + kd) * texture(textureVoter, vTexCoords + vec2(time * 0.05 ,time * 0.05)).rgb, 1.0f);
    //color = vec4(vec3(0.0f,0.2f,1.0f), 1.0f);
  //color = mix(fog_color, color, fogFactor);
  }
  if(FogState){
    color = mix(fog_color, color, fogFactor);
  }

  if(normalview == 1){
    color = vec4(0.5f * vNormal + vec3(0.5f, 0.5f, 0.5f), 1.0f); 
  }
}