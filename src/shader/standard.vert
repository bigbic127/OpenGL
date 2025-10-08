#version 410 core
layout(location = 0) in vec3 vPosition;
uniform mat4 mModel, mView, mProjection;
void main()
{
    gl_Position = mProjection * mView * mModel * vec4(vPosition, 1.0);
}