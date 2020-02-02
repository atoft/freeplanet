@Vertex

#version 130
in vec3 frplPosition;
in vec3 frplNormal;
in vec2 frplTexcoord;

uniform vec3 frplBaseColor;
uniform mat4 frplTransform;
uniform mat4 frplModelTransform;
uniform mat4 frplNormalTransform;
uniform mat4 frplCameraRotation;
uniform vec3 frplCameraWorldPosition;
uniform float frplAspectRatio;

out vec3 Color;
out vec3 WorldPosition;
out vec3 CameraWorldPosition;
out vec3 Normal;
out vec2 TexCoord;

uniform sampler2D tex2D_0;

void main()
{
    Color = frplBaseColor;
    TexCoord = frplTexcoord;

    WorldPosition = (frplModelTransform * vec4(frplPosition, 1.0)).xyz ;
    gl_Position = frplTransform * vec4(frplPosition, 1.0);
    gl_Position = gl_Position.xyww;

    vec4 norm = frplNormalTransform * vec4((TexCoord.x * 2.0 - 1.0) * frplAspectRatio, 2.0 * TexCoord.y - 1.0, -1.0, 1.0);
    Normal = normalize(norm.xyz);
}


@Fragment
#version 130
in vec3 Color;
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
    vec3 normal = normalize(Normal);

    vec3 DAY_SKY_COLOR = vec3(0.078, 0.764, 1);
    vec3 NIGHT_SKY_COLOR = vec3(0.078, 0.109, 0.301);

    vec3 DAY_HAZE_COLOR = vec3(0.2, 0.325, 0.4);
    vec3 NIGHT_HAZE_COLOR = vec3(0.113, 0.184, 0.223);

    vec3 SUNSET_COLOR = vec3(0.713, 0.462, 0.007);

    vec3 SUN_GLOW_COLOR = vec3(0.2, 0.2, 0.2);

    float sunDisk = dot(frplDirectionalLight.direction, normal);
    float sunIntensity = smoothstep(0.9975, 0.998, sunDisk);
    float hazeIntensity = smoothstep(0.8, 1.0, 1 - clamp(dot(normal, vec3(0,1,0)), 0, 1));


    float sunGlowIntensity = smoothstep(0.8, 1, sunDisk);
    vec3 sunGlowColor = sunGlowIntensity * SUN_GLOW_COLOR;

    float nightExtent = clamp(dot(frplDirectionalLight.direction, vec3(0,1,0) + 0.2), 0, 1);

    vec3 hazeColor = mix(NIGHT_HAZE_COLOR, DAY_HAZE_COLOR, nightExtent) * hazeIntensity;
    vec3 skyColor = mix(NIGHT_SKY_COLOR, DAY_SKY_COLOR, nightExtent);

    float sunsetIntensity = smoothstep(0.3, 1.0, 1 - clamp(dot(normal, vec3(0,1,0)), 0, 1));
    float sunsetExtent =  smoothstep(0.4, 0, abs(dot(frplDirectionalLight.direction, vec3(0,1,0))));
    vec3 sunsetColor = sunsetIntensity * sunsetExtent * SUNSET_COLOR;

    outColor = vec4(sunIntensity * frplDirectionalLight.color + sunGlowColor + skyColor + hazeColor + sunsetColor, 1.0);
}
