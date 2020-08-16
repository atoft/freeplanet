@Vertex

#version 130
in vec3 frplPosition;
in vec3 frplNormal;
in vec2 frplTexcoord;
in vec3 frplColor;

uniform vec3 frplBaseColor;
uniform mat4 frplTransform;
uniform mat4 frplModelTransform;
uniform mat4 frplNormalTransform;
uniform vec3 frplLocalUpDirection;
uniform vec3 frplCameraWorldPosition;


out vec3 Color;
out vec3 WorldPosition;
out vec3 CameraWorldPosition;
out vec3 Normal;
out vec3 LocalUpDirection;
out vec2 TexCoord;

uniform sampler2D tex2D_0;

void main()
{
    Color = frplColor;
    TexCoord = frplTexcoord;
    LocalUpDirection = frplLocalUpDirection;

    WorldPosition = (frplModelTransform * vec4(frplPosition, 1.0)).xyz ;
    gl_Position = frplTransform * vec4(frplPosition, 1.0);
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
in vec3 LocalUpDirection;

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
    vec3 sunDiffuse = clamp(dot(Normal, frplDirectionalLight.direction), 0, 1) * frplDirectionalLight.color * frplDirectionalLight.intensity;

    vec3 ambient = clamp(dot(Normal, LocalUpDirection), 0.5, 1) * frplAmbientLight.color * frplAmbientLight.intensity;

    vec3 pointLighting = vec3(0,0,0);

    for (int lightIdx = 0; lightIdx < 8; ++lightIdx)
    {
        vec3 fragToLight = frplLights[lightIdx].position - WorldPosition;

        float pointBrightness = dot(Normal, fragToLight) / (length(fragToLight));
        pointBrightness = clamp(pointBrightness, 0.0, 1.0);

        pointLighting = pointLighting + vec3(length(fragToLight) / 10.0);//pointLighting + vec3(pointBrightness * 0.1);// * frplLights[lightIdx].color * frplLights[lightIdx].brightness);
        break;
    }

    outColor = vec4(pointLighting.rgb, 1.0);//vec4((ambient + sunDiffuse + pointLighting) * Color,1.0);
}
