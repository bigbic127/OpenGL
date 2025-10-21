#version 410 core
layout(location = 0) in vec3 vPosition;
layout(location = 1) in vec3 vNormal;
layout(location = 2) in vec2 vTexCoord;
uniform mat4 mModel, mView, mProjection;
out vec3 fragWorldPos;
void main()
{
    fragWorldPos =  vec3(mModel * vec4(vPosition, 1.0f));
    gl_Position = mProjection * mView * mModel * vec4(vPosition, 1.0f);
}