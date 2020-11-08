@Vertex

#version 130
in vec3 frplPosition;
in vec3 frplNormal;
in vec2 frplTexcoord;
in vec3 frplColor;
in vec4 frplTerrainSubstance; // HACK
in mat4 frplInstanceTransform;

uniform vec3 frplBaseColor;
// uniform mat4 frplTransform;
uniform mat4 frplModelTransform;
uniform mat4 frplNormalTransform;
uniform vec3 frplCameraWorldPosition;


out vec3 Color;
out vec3 WorldPosition;
out vec3 CameraWorldPosition;
out vec3 Normal;
out vec2 TexCoord;

uniform sampler2D tex2D_0;

void main()
{
    Color = frplColor;
    TexCoord = frplTexcoord;


    WorldPosition = (frplModelTransform * vec4(frplPosition, 1.0)).xyz ;
    gl_Position = frplInstanceTransform * vec4(frplPosition, 1.0);
    CameraWorldPosition = frplCameraWorldPosition;

    vec4 norm = frplNormalTransform * vec4(frplNormal, 1.0);
    Normal = norm.xyz / norm.w;
}

@Fragment

#version 130
in vec3 Color;
in vec3 WorldPosition;
in vec3 CameraWorldPosition;
in vec3 Normal;
in vec2 TexCoord;

uniform sampler2D tex2D_0;
uniform struct Light {
   vec3 position;
   vec3 color;
   float brightness;
};
uniform struct DirectionalLight {
   vec3 direction;
   vec3 color;
   float intensity;
};
uniform struct AmbientLight {
   vec3 color;
   float intensity;
};

uniform Light frplLights[8];
uniform DirectionalLight frplDirectionalLight;
uniform AmbientLight frplAmbientLight;



out vec4 outColor;


void main()
{
    vec4 texel = texture(tex2D_0, TexCoord);

    if (texel.a < 0.9)
    {
        discard;
    }

    vec3 FIXED_NORMAL = vec3(0,1,0);

    vec3 sunDiffuse = clamp(dot(FIXED_NORMAL, frplDirectionalLight.direction), 0, 1) * frplDirectionalLight.color * frplDirectionalLight.intensity;

    vec3 ambient = clamp(dot(FIXED_NORMAL, vec3(0,1,0)), 0.5, 1) * frplAmbientLight.color * frplAmbientLight.intensity;

    outColor = vec4((ambient + sunDiffuse), 1.0) * texel;
}
