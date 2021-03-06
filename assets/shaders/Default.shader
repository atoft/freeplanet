@Vertex

#version 130
in vec3 frplPosition;
in vec3 frplNormal;
in vec2 frplTexcoord;
in vec3 frplColor;
in vec4 frplTerrainSubstance;
in mat4 frplInstanceTransform;
in mat4 frplInstanceNormalTransform;

uniform vec3 frplBaseColor;
uniform mat4 frplTransform;
uniform mat4 frplModelTransform;
uniform mat4 frplNormalTransform;
uniform vec3 frplCameraWorldPosition;


out vec3 Color;
out vec4 TerrainSubstance;
out vec3 WorldPosition;
out vec3 CameraWorldPosition;
out vec3 Normal;
out vec2 TexCoord;

uniform sampler2D tex2D_0;

void main()
{
    Color = frplBaseColor + frplColor;
    TexCoord = frplTexcoord;
    TerrainSubstance = frplTerrainSubstance;

    WorldPosition = (frplModelTransform * frplInstanceTransform * frplInstanceNormalTransform * vec4(frplPosition, 1.0)).xyz ;
    gl_Position = frplTransform * vec4(frplPosition, 1.0);
    CameraWorldPosition = frplCameraWorldPosition;

    vec4 norm = frplNormalTransform * vec4(frplNormal, 1.0);
    Normal = norm.xyz / norm.w;
}

@Fragment

#version 130
in vec3 Color;
in vec4 TerrainSubstance;
in vec3 WorldPosition;
in vec3 CameraWorldPosition;
in vec3 Normal;
in vec2 TexCoord;

uniform sampler2D tex2D_0;
uniform samplerCube texCUBE_0;

uniform struct Light {
   vec3 position;
   vec3 color;
   float brightness;
};

uniform Light frplLights[8];
uniform int frplActiveLights;


out vec4 outColor;


void main()
{
    outColor = vec4(Normal + WorldPosition, 1.0)* 0.5 + texture(tex2D_0, TexCoord) + vec4(TexCoord, 0.0, 1.0) + vec4(Color, 1.0) + TerrainSubstance;
}
