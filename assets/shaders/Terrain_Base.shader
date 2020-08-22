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

uniform sampler3D texPerlin;
uniform sampler3D texGrass;

uniform struct Light {
   vec3 position;
   vec3 color;
   float intensity;
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

float Remap(float _value, float _min, float _max)
{
    return max(_value - _min, 0.0) / (_max - _min);
}

vec3 GetGrassColor()
{
    float baseColor = texture(texGrass, WorldPosition.xzy).r;

    const float GRASS_CONTRAST = 0.6;
    const float GRASS_BRIGHTNESS = 0.5;

    float scaledColor = (baseColor * GRASS_CONTRAST * GRASS_BRIGHTNESS) + (1.0 - GRASS_CONTRAST);

    const vec3 GRASS_COLOR = vec3(0.521, 0.835, 0.301);

    return GRASS_COLOR * vec3(scaledColor);
}

vec3 GetDirtColor()
{
    float baseColor = texture(texPerlin, WorldPosition.xzy / 2.0).r * 0.5 + texture(texPerlin, WorldPosition.xzy).r * 0.5;

    const float DIRT_CONTRAST = 0.4;
    const float DIRT_BRIGHTNESS = 1.0;

    float scaledColor = (baseColor * DIRT_CONTRAST * DIRT_BRIGHTNESS) + (1.0 - DIRT_CONTRAST);

    const vec3 DIRT_COLOR = vec3(0.737, 0.407, 0.101);

    return DIRT_COLOR * vec3(scaledColor);
}

vec3 GetSurfaceColor()
{
    const float MASK_SCALE = 32.0;
    float foliageMask = texture(texPerlin, WorldPosition.xzy / MASK_SCALE).r;

    const float DETAIL_SCALE1 = 8.0;
    const float DETAIL_SCALE2 = 2.0;
    float maskDetail = texture(texPerlin, WorldPosition.xzy / DETAIL_SCALE1).r * 0.5 + texture(texPerlin, WorldPosition.xzy / DETAIL_SCALE2).r * 0.5;

    foliageMask = foliageMask * maskDetail;

    // Extent of base mask depends on steepness of terrain (e.g. more grass if more flat).
    float verticalDotProduct = clamp(dot(Normal, LocalUpDirection), 0, 1);

    // Angle above which there can be no foliage.
    float MAX_GRASS_ANGLE = 0.2;

    // Angle below which there is full foliage.
    float MIN_GRASS_ANGLE = 0.8;

    float foliageExtent = Remap(verticalDotProduct, MAX_GRASS_ANGLE, MIN_GRASS_ANGLE);

    // Global foliage amount.
    float FOLIAGE_COVERAGE = 0.8;

    // How sharp are the edges of foliage transitions.
    float FOLIAGE_SOFTNESS = 0.2;

    float foliageCoverageAccountingForSlope = FOLIAGE_COVERAGE * foliageExtent;

    foliageMask = foliageCoverageAccountingForSlope > 0.0 ? Remap(foliageMask, 1.0 - foliageCoverageAccountingForSlope, min((1.0 - foliageCoverageAccountingForSlope) + FOLIAGE_SOFTNESS, 1.0)) : 0.0;

    vec3 grassColor = GetGrassColor() * foliageMask;
    vec3 dirtColor = GetDirtColor() * (1.0 - foliageMask);

    return grassColor + dirtColor;
}

void main()
{
    vec3 sunDiffuse = clamp(dot(Normal, frplDirectionalLight.direction), 0, 1) * frplDirectionalLight.color * frplDirectionalLight.intensity;

    vec3 ambient = clamp(dot(Normal, LocalUpDirection), 0.5, 1) * frplAmbientLight.color * frplAmbientLight.intensity;

    vec3 pointLighting = vec3(0,0,0);

    for (int lightIdx = 0; lightIdx < 8; ++lightIdx)
    {
        vec3 fragToLight = frplLights[lightIdx].position - WorldPosition;

        float pointBrightness = dot(Normal, normalize(fragToLight));
        pointBrightness = clamp(pointBrightness, 0.0, 1.0);

        float distanceToLight = length(fragToLight);
        pointLighting = pointLighting + (1.0 / (distanceToLight * distanceToLight)) * frplLights[lightIdx].color * frplLights[lightIdx].intensity * pointBrightness;
        break;
    }

    vec3 surfaceColor = GetSurfaceColor();
    outColor = vec4((ambient + sunDiffuse + pointLighting) * surfaceColor,1.0);
}
