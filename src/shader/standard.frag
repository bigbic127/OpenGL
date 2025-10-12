#version 410 core
#define LIGHT_TYPE_DIRECTIONAL 0
#define LIGHT_TYPE_SPOT 1
#define LIGHT_TYPE_POINT 2

out vec4 FragColor;
in vec3 fragWorldPos;
in vec3 fragNormal;
in vec2 fragTexCoord;
//light
uniform int lightType = 0;
uniform vec3 lightPosition;
uniform float lightIntensity = 1.0f;
uniform vec3 lightColor = vec3(1.0f, 1.0f, 1.0f);
uniform vec3 directionalLight = vec3(0.0f, 1.0f, 0.0f);
//camera
uniform vec3 cameraPosition;
//parameter
uniform vec3 ambientColor = vec3(0.0f, 0.0f, 0.0);
uniform vec3 baseColor = vec3(1.0f, 1.0f, 1.0f);
uniform vec3 specularColor = vec3(0.5f, 0.5f, 0.5f);
uniform float specularPow = 32.0f;
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
    vec3 ambient = lightColor * ambientColor * baseColor * diffuseTextureColor;
    //difuse
    vec3 diffuseResult = lightIntensity * lightValue * lightColor  * baseColor * diffuseTextureColor;
    //specular
    vec3 specularDir = normalize(cameraPosition - fragWorldPos);
    vec3 reflectDir = reflect(-lightDir, normal);
    float reflectValue = pow(max(dot(specularDir, reflectDir),0.0f), specularPow);
    vec3 specularResult = reflectValue * lightColor * specularColor * specularTextureColor;

    vec3 finalColor = ambient + diffuseResult + specularResult;
    FragColor = vec4(finalColor,1.0);
}