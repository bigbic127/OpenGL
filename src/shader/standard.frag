#version 410 core
#define MAX_LIGHTS 8
#define LIGHT_TYPE_DIRECTIONAL 0
#define LIGHT_TYPE_SPOT 1
#define LIGHT_TYPE_POINT 2

out vec4 FragColor;
in vec3 fragWorldPos;
in vec3 fragNormal;
in vec2 fragTexCoord;
//light
struct DirectionalLight
{
    vec3 direction;
    vec3 color;
    float intensity;
};
struct Light
{
    vec3 position;
    vec3 direction;
    vec3 color;
    float intensity;
    float constant;
    float linear;
    float quadratic;
    float cutOff;
    float outerCutOff;
    int type;
};
//uniform DirectionalLight directionalLight;
uniform int maxLights = 0;
uniform int lightType = 0;
uniform vec3 lightPosition;
uniform float lightIntensity = 1.0f;
uniform vec3 lightColor = vec3(1.0f, 1.0f, 1.0f);
uniform vec3 directionalLight = vec3(0.0f, 1.0f, 0.0f);
uniform float lightConstant = 1.0f;
uniform float lightLinear = 0.7f;
uniform float lightQuadratic = 1.8f;
uniform float lightCutoff = 0.966f;
uniform float lightOuterCutoff = 0.953f;
//camera
uniform vec3 cameraPosition;
//parameter
uniform vec3 ambientColor = vec3(0.0f, 0.0f, 0.0);
uniform vec3 baseColor = vec3(1.0f, 1.0f, 1.0f);
uniform vec3 specularColor = vec3(0.5f, 0.5f, 0.5f);
uniform float specularShininess = 32.0f;
//texture
uniform bool bDiffuse = false;
uniform sampler2D diffuseTexture;
uniform bool bSpecular = false;
uniform sampler2D specularTexture;

void main()
{
    vec3 normal = normalize(fragNormal);
    //Texture
    vec3 diffuseTextureColor = vec3(1.0f, 1.0f, 1.0f);
    if(bDiffuse)
        diffuseTextureColor = vec3(texture(diffuseTexture, fragTexCoord).rgb);
    vec3 specularTextureColor = vec3(1.0f, 1.0f, 1.0f);
    if(bSpecular)
        specularTextureColor = vec3(texture(specularTexture, fragTexCoord).rgb);
    //light
    vec3 lightDir = normalize(directionalLight);
    if(lightType == LIGHT_TYPE_SPOT)
        vec3 lightDir = normalize(lightPosition - fragWorldPos);
    else if(lightType == LIGHT_TYPE_POINT){}
    float lightValue = max(dot(normal, lightDir),0.0f);
    //ambient
    vec3 ambientResult = lightColor * ambientColor * baseColor * diffuseTextureColor;
    //difuse
    vec3 diffuseResult = lightIntensity * lightValue * lightColor  * baseColor * diffuseTextureColor;
    //specular
    vec3 specularDir = normalize(cameraPosition - fragWorldPos);
    vec3 reflectDir = reflect(-lightDir, normal);
    float reflectValue = pow(max(dot(specularDir, reflectDir),0.0f), specularShininess);
    vec3 specularResult = reflectValue * lightColor * specularColor * specularTextureColor;
    //attenuation
    float attenuation = 1.0f;
    float attenuationIntensity = 1.0f;
    if(lightType == LIGHT_TYPE_SPOT)
    {
        float dist = length(lightPosition - fragWorldPos);
        attenuation = 1.0 / (lightConstant + lightLinear * dist + lightQuadratic *(dist * dist));
        //Intensity
        float theta = dot(lightDir, normalize(-directionalLight));
        float epsilon = lightCutoff - lightOuterCutoff;
        attenuationIntensity = clamp((theta - lightOuterCutoff)/epsilon, 0.0f, 1.0f);
    }
    ambientResult *= attenuation * attenuationIntensity;
    diffuseResult *= attenuation * attenuationIntensity;
    specularResult *= attenuation * attenuationIntensity;

    vec3 finalColor = ambientResult + diffuseResult + specularResult;
    FragColor = vec4(finalColor, 1.0f);
}