#version 410 core
out vec4 FragColor;
in vec3 fragWorldPos;
in vec3 fragNormal;
in vec2 fragTexCoord;
//light
uniform vec3 lightPosition;
uniform float lightIntensity = 1.0f;
uniform vec3 lightColor = vec3(1.0f, 1.0f, 1.0f);
uniform vec3 directionalLight = vec3(0.0f, 1.0f, 0.0f);
//camera
uniform vec3 cameraPosition;
//parameter
uniform vec3 ambientColor = vec3(0.0f, 0.0f, 0.0);
uniform vec3 baseColor = vec3(1.0f, 1.0f, 1.0f);
//texture
uniform bool bDiffuse = false;
uniform sampler2D diffuseTexture;

void main()
{
    //diffuseTexture
    vec3 diffuseColor = vec3(1.0f, 1.0f, 1.0f);
    if(bDiffuse)
        diffuseColor = vec3(texture(diffuseTexture, fragTexCoord).rgb);
    //light
    vec3 normal = normalize(fragNormal);
    vec3 lightDir = normalize(directionalLight);
    float lightValue = max(dot(normal, lightDir),0.0f);
    //ambient
    vec3 ambient = lightColor * ambientColor * baseColor * diffuseColor;
    //difuse
    vec3 diffuseResult = lightIntensity * lightValue * lightColor  * baseColor * diffuseColor;

    vec3 finalColor = ambient + diffuseResult;
    FragColor = vec4(finalColor,1.0);
}