#version 410 core
out vec4 FragColor;
in vec3 fragWorldPos;
uniform vec3 cameraPosition;
//parameter
uniform vec3 backgroundColor = vec3(0.10, 0.10, 0.12);
uniform vec3 gridColor = vec3(0.3, 0.3, 0.3);
uniform vec3 axisXColor  = vec3(0.6, 0.0, 0.0);
uniform vec3 axisZColor = vec3(0.0, 0.4, 0.8);


float gridLine(float coord, float scale)
{
    float line = abs(fract(coord/scale) - 0.5);
    return smoothstep(0.48, 0.5, line); // 선 두께 조절
}

void main()
{
    vec2 pos = fragWorldPos.xz;
    //기본 그리드 라인
    float grid1 = gridLine(pos.x, 1.0);
    float grid2 = gridLine(pos.y, 1.0);
    float grid = mix(grid1, grid2, 0.5);
    vec3 color = mix(backgroundColor, gridColor, grid);
    //XY축 색상
    if (abs(fragWorldPos.x) < 0.03) color = axisXColor;
    if (abs(fragWorldPos.z) < 0.03) color = axisZColor;
    //거리에 따른 감쇠 (fade-out)
    float dist = length(fragWorldPos.xz - cameraPosition.xz);
    float alpha = 1.0 - smoothstep(50.0, 100.0, dist);
    FragColor = vec4(color, alpha);
}