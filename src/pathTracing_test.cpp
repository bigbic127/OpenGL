// main.cpp
// OpenGL progressive path tracer (compute shader) + rotating red box + reflective sphere
// Requires: GLFW, GLAD, GLM
// Build example (Linux): g++ -std=c++17 main.cpp -lglfw -ldl -lGL -pthread -o pathtracer

#include <glad/glad.h>
#include <glfw/glfw3.h>
#include <glm.hpp>
#include <gtc/type_ptr.hpp>
#include <gtc/matrix_transform.hpp>

#include <iostream>
#include <vector>
#include <string>
#include <chrono>
#include <cmath>

#define M_PI 3.14159265358979323846

#define CHECK_GL() do{ GLenum e = glGetError(); if(e!=GL_NO_ERROR) std::cerr<<"GLERR "<<e<<"\n"; } while(0)

static const int WIN_W = 1280;
static const int WIN_H = 720;

// ---------- fullscreen quad ----------
static GLuint quadVAO = 0;
static void createFullscreenQuad(){
    if(quadVAO) return;
    float quad[] = {
        -1.f, -1.f, 0.f, 0.f, 0.f,
         1.f, -1.f, 0.f, 1.f, 0.f,
         1.f,  1.f, 0.f, 1.f, 1.f,
        -1.f,  1.f, 0.f, 0.f, 1.f
    };
    unsigned int quadIdx[] = {0,1,2, 0,2,3};
    GLuint VBO, EBO;
    glGenVertexArrays(1,&quadVAO);
    glGenBuffers(1,&VBO); glGenBuffers(1,&EBO);
    glBindVertexArray(quadVAO);
    glBindBuffer(GL_ARRAY_BUFFER,VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(quad), quad, GL_STATIC_DRAW);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER,EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(quadIdx), quadIdx, GL_STATIC_DRAW);
    glEnableVertexAttribArray(0); glVertexAttribPointer(0,3,GL_FLOAT,GL_FALSE,5*sizeof(float),(void*)0);
    glEnableVertexAttribArray(1); glVertexAttribPointer(1,2,GL_FLOAT,GL_FALSE,5*sizeof(float),(void*)(3*sizeof(float)));
    glBindVertexArray(0);
}

// ---------- simple shader utility ----------
static GLuint compileShader(GLenum type, const char* src){
    GLuint s = glCreateShader(type);
    glShaderSource(s,1,&src,nullptr);
    glCompileShader(s);
    GLint ok; glGetShaderiv(s, GL_COMPILE_STATUS, &ok);
    if(!ok){ char log[2048]; glGetShaderInfoLog(s,2048,nullptr,log); std::cerr<<"Shader compile error: "<<log<<"\n"; }
    return s;
}
static GLuint linkProgram(GLuint vs, GLuint fs){
    GLuint p = glCreateProgram();
    glAttachShader(p, vs); glAttachShader(p, fs);
    glLinkProgram(p);
    GLint ok; glGetProgramiv(p, GL_LINK_STATUS, &ok);
    if(!ok){ char log[2048]; glGetProgramInfoLog(p,2048,nullptr,log); std::cerr<<"Link error: "<<log<<"\n"; }
    return p;
}
static GLuint createProgramFromSrc(const char* vsSrc, const char* fsSrc){
    GLuint vs = compileShader(GL_VERTEX_SHADER, vsSrc);
    GLuint fs = compileShader(GL_FRAGMENT_SHADER, fsSrc);
    GLuint p = linkProgram(vs,fs);
    glDeleteShader(vs); glDeleteShader(fs);
    return p;
}
static GLuint createComputeFromSrc(const char* csSrc){
    GLuint cs = compileShader(GL_COMPUTE_SHADER, csSrc);
    GLuint p = glCreateProgram(); glAttachShader(p, cs); glLinkProgram(p);
    GLint ok; glGetProgramiv(p, GL_LINK_STATUS, &ok);
    if(!ok){ char log[4096]; glGetProgramInfoLog(p,4096,nullptr,log); std::cerr<<"Compute link error: "<<log<<"\n"; }
    glDeleteShader(cs);
    return p;
}

// ---------- display shader (draw accum texture) ----------
const char* vs_display = R"(
#version 330 core
layout(location=0) in vec3 aPos;
layout(location=1) in vec2 aUV;
out vec2 uv;
void main(){ uv=aUV; gl_Position = vec4(aPos,1.0); }
)";
const char* fs_display = R"(
#version 330 core
in vec2 uv;
out vec4 FragColor;
uniform sampler2D accumTex;
uniform int frameCount;
void main(){
    vec3 c = texture(accumTex, uv).rgb;
    // simple tonemap + gamma
    vec3 mapped = c / (c + vec3(1.0));
    mapped = pow(mapped, vec3(1.0/2.2));
    FragColor = vec4(mapped, 1.0);
}
)";

// ---------- compute shader: path tracer ----------
const char* cs_pathtracer = R"(
#version 430
layout (local_size_x = 16, local_size_y = 16) in;

layout(rgba32f, binding = 0) uniform image2D accumImage;

uniform ivec2 iResolution;
uniform int uFrame; // current accumulated frames (reads previous result)
uniform float uTime;
uniform vec3 uCamPos;
uniform vec3 uCamForward;
uniform vec3 uCamRight;
uniform vec3 uCamUp;
uniform float uFov; // in radians

// scene params (box rotation)
uniform float uBoxAngle; // radians

// simple RNG (xorshift)
uint wangHash(uint s){
    s = (s ^ 61u) + (s << 3);
    s = s ^ (s >> 16);
    s *= 9u;
    s = s ^ (s >> 4);
    s *= 0x27d4eb2du;
    s = s ^ (s >> 15);
    return s;
}
float rnd(inout uint state){
    state = wangHash(state);
    return float(state) / 4294967296.0;
}

// structures: ray, hit
struct Ray{ vec3 o,d; };
struct Hit{ float t; vec3 p,n; int mat; bool hit; };

// simple intersect: sphere, plane, rotated box (approx by transforming ray to local box space)
Hit hitSphere(Ray r, vec3 center, float radius, int mat){
    Hit h; h.hit=false; h.t=1e20;
    vec3 oc = r.o - center;
    float a = dot(r.d,r.d);
    float b = dot(oc,r.d);
    float c = dot(oc,oc) - radius*radius;
    float disc = b*b - a*c;
    if(disc < 0.0) return h;
    float sd = sqrt(disc);
    float t = (-b - sd)/a;
    if(t < 0.001){ t = (-b + sd)/a; if(t < 0.001) return h; }
    h.hit = true; h.t = t;
    h.p = r.o + r.d * t;
    h.n = normalize(h.p - center);
    h.mat = mat;
    return h;
}

// axis-aligned box intersection in object space (centered at origin, extents)
Hit hitAABB(Ray r, vec3 mn, vec3 mx, int mat){
    // slab method
    vec3 invd = vec3(1.0)/r.d;
    vec3 t0s = (mn - r.o) * invd;
    vec3 t1s = (mx - r.o) * invd;
    vec3 tsm = min(t0s,t1s);
    vec3 tb = max(t0s,t1s);
    float tmin = max(max(tsm.x, tsm.y), max(tsm.z, 0.001));
    float tmax = min(min(tb.x, tb.y), tb.z);
    Hit h; h.hit=false;
    if(tmin <= tmax){
        h.hit=true; h.t=tmin;
        h.p = r.o + r.d*h.t;
        // compute normal by checking which slab
        const float eps=1e-4;
        if(abs(h.p.x - mn.x) < eps) h.n = vec3(-1,0,0);
        else if(abs(h.p.x - mx.x) < eps) h.n = vec3(1,0,0);
        else if(abs(h.p.y - mn.y) < eps) h.n = vec3(0,-1,0);
        else if(abs(h.p.y - mx.y) < eps) h.n = vec3(0,1,0);
        else if(abs(h.p.z - mn.z) < eps) h.n = vec3(0,0,-1);
        else h.n = vec3(0,0,1);
        h.mat = mat;
    }
    return h;
}

// rotate point by Y
mat3 rotY(float a){
    float c = cos(a), s = sin(a);
    return mat3( vec3(c,0,s), vec3(0,1,0), vec3(-s,0,c) );
}

// hit world: we will define materials as integers:
// 0: room diffuse grey
// 1: red box diffuse
// 2: metal sphere (reflective)
// 3: emissive light
Hit sceneIntersect(Ray r){
    Hit best; best.hit=false; best.t=1e20;
    // Room: we model as 5 planes (back, floor, ceiling, left, right)
    // back wall z = -1.8  (normal +z interior)
    {
        float zplane=-1.8;
        float denom = dot(r.d, vec3(0,0,1));
        if(abs(denom) > 1e-6){
            float t = (zplane - r.o.z) / r.d.z;
            if(t>0.001 && t < best.t){
                vec3 p = r.o + r.d*t;
                if(p.x >= -1.6 && p.x <= 1.6 && p.y >= -1.1 && p.y <= 1.1){
                    best.hit=true; best.t=t; best.p=p; best.n=vec3(0,0,1); best.mat=0;
                }
            }
        }
    }
    // floor y = -1.1  normal +y
    {
        float yplane = -1.1;
        if(abs(r.d.y) > 1e-6){
            float t = (yplane - r.o.y)/r.d.y;
            if(t>0.001 && t < best.t){
                vec3 p = r.o + r.d*t;
                if(p.x >= -1.6 && p.x <= 1.6 && p.z >= -1.8 && p.z <= 1.8){
                    best.hit=true; best.t=t; best.p=p; best.n=vec3(0,1,0); best.mat=0;
                }
            }
        }
    }
    // ceiling y = +1.1 (normal -y)
    {
        float yplane = 1.1;
        if(abs(r.d.y) > 1e-6){
            float t = (yplane - r.o.y)/r.d.y;
            if(t>0.001 && t < best.t){
                vec3 p = r.o + r.d*t;
                if(p.x >= -1.6 && p.x <= 1.6 && p.z >= -1.8 && p.z <= 1.8){
                    best.hit=true; best.t=t; best.p=p; best.n=vec3(0,-1,0); best.mat=0;
                }
            }
        }
    }
    // left wall x = -1.6 (normal +x)
    {
        float xplane = -1.6;
        if(abs(r.d.x) > 1e-6){
            float t = (xplane - r.o.x)/r.d.x;
            if(t>0.001 && t < best.t){
                vec3 p = r.o + r.d*t;
                if(p.y >= -1.1 && p.y <= 1.1 && p.z >= -1.8 && p.z <= 1.8){
                    best.hit=true; best.t=t; best.p=p; best.n=vec3(1,0,0); best.mat=0;
                }
            }
        }
    }
    // right wall x = +1.6 (normal -x)
    {
        float xplane = 1.6;
        if(abs(r.d.x) > 1e-6){
            float t = (xplane - r.o.x)/r.d.x;
            if(t>0.001 && t < best.t){
                vec3 p = r.o + r.d*t;
                if(p.y >= -1.1 && p.y <= 1.1 && p.z >= -1.8 && p.z <= 1.8){
                    best.hit=true; best.t=t; best.p=p; best.n=vec3(-1,0,0); best.mat=0;
                }
            }
        }
    }

    // emissive small quad (ceiling area) (we'll approximate as small rectangle at y=0.99)
    {
        float yplane = 0.99;
        if(abs(r.d.y) > 1e-6){
            float t = (yplane - r.o.y)/r.d.y;
            if(t>0.001 && t < best.t){
                vec3 p = r.o + r.d*t;
                if(p.x >= -0.25 && p.x <= 0.25 && p.z >= -0.25 && p.z <= 0.25){
                    best.hit=true; best.t=t; best.p=p; best.n=vec3(0,-1,0); best.mat=3;
                }
            }
        }
    }

    // red box: rotated around Y by uBoxAngle, box local coords (min,max)
    {
        // rotate ray into box local space: R^T * (r.o - boxPos)
        vec3 boxPos = vec3(0.5, -0.6, -0.3);
        float angle = uBoxAngle;
        mat3 R = rotY(angle);
        mat3 Rt = transpose(R);
        vec3 ro_local = Rt * (r.o - boxPos);
        vec3 rd_local = Rt * r.d;
        Ray rloc = Ray(ro_local, rd_local);
        Hit hb = hitAABB(rloc, vec3(-0.35,-0.25,-0.3), vec3(0.35,0.25,0.3), 1);
        if(hb.hit && hb.t < best.t){
            // transform hit back to world
            vec3 p_local = rloc.o + rloc.d * hb.t;
            vec3 n_local = hb.n;
            vec3 pw = R * p_local + boxPos;
            vec3 nw = normalize(R * n_local);
            best.hit=true; best.t = length(pw - r.o); best.p = pw; best.n = nw; best.mat = 1;
        }
    }

    // metallic sphere
    {
        vec3 sphPos = vec3(-0.7, -0.65, 0.2);
        Hit hs = hitSphere(r, sphPos, 0.45, 2);
        if(hs.hit && hs.t < best.t) { best = hs; }
    }

    return best;
}

// material properties fetch
// 0: room -> grey diffuse
// 1: red box -> diffuse red
// 2: metal -> high reflect
// 3: emissive -> light color
vec3 materialAlbedo(int m){
    if(m==0) return vec3(0.65,0.65,0.65);
    if(m==1) return vec3(0.85,0.1,0.1);
    if(m==2) return vec3(0.95,0.93,0.9); // metal tint
    return vec3(0.0);
}
float materialRoughness(int m){
    if(m==2) return 0.02;
    return 1.0; // diffuse
}
vec3 materialEmit(int m){
    if(m==3) return vec3(18.0, 14.0, 10.0); // bright bulb
    return vec3(0.0);
}

// sample hemisphere cosine-weighted
vec3 sampleHemisphere(vec3 n, inout uint rng){
    float u1 = rnd(rng);
    float u2 = rnd(rng);
    float r = sqrt(u1);
    float phi = 2.0*3.14159265359*u2;
    vec3 local = vec3(r*cos(phi), r*sin(phi), sqrt(max(0.0,1.0 - u1)));
    // create tangent frame
    vec3 up = abs(n.z) < 0.999 ? vec3(0,0,1) : vec3(1,0,0);
    vec3 tangent = normalize(cross(up, n));
    vec3 bitangent = cross(n, tangent);
    return normalize(local.x * tangent + local.y * bitangent + local.z * n);
}

// Fresnel Schlick for metals approx - input F0
vec3 fresnelSchlick(float cosTheta, vec3 F0){
    return F0 + (1.0 - F0) * pow(1.0 - cosTheta, 5.0);
}

void main(){
    ivec2 pix = ivec2(gl_GlobalInvocationID.xy);
    if(pix.x >= iResolution.x || pix.y >= iResolution.y) return;
    ivec2 res = iResolution;

    uint seed = uint(pix.x + pix.y*res.x) ^ uint(uFrame*1993) ^ uint(floatBitsToInt(uTime*917.0));
    // jitter within pixel
    float rx = rnd(seed);
    float ry = rnd(seed);

    // generate primary ray in camera space
    float ndc_x = ( (float(pix.x) + rx) / float(res.x) ) * 2.0 - 1.0;
    float ndc_y = ( (float(pix.y) + ry) / float(res.y) ) * 2.0 - 1.0;
    float aspect = float(res.x)/float(res.y);
    float px = ndc_x * tan(uFov*0.5) * aspect;
    float py = ndc_y * tan(uFov*0.5);
    vec3 rd = normalize( px * uCamRight + py * uCamUp + uCamForward );

    Ray ray; ray.o = uCamPos; ray.d = rd;

    vec3 throughput = vec3(1.0);
    vec3 accum = vec3(0.0);

    // simple path tracer: up to 4 bounces
    for(int bounce=0; bounce<5; ++bounce){
        Hit h = sceneIntersect(ray);
        if(!h.hit){
            // background - dark
            accum += throughput * vec3(0.02,0.02,0.03);
            break;
        }
        // emissive
        vec3 emit = materialEmit(h.mat);
        if(length(emit) > 0.0){
            accum += throughput * emit;
            break;
        }

        vec3 albedo = materialAlbedo(h.mat);
        float rough = materialRoughness(h.mat);

        // direct lighting (one-sample towards the bulb area)
        // sample point on emitter (small square on ceiling)
        // approximate by sampling center
        vec3 lightPos = vec3(0.0, 0.99, -0.1);
        vec3 toL = lightPos - h.p;
        float dist2 = dot(toL, toL);
        vec3 Ldir = normalize(toL);
        // shadow ray simple visibility
        Ray shadow; shadow.o = h.p + h.n*0.001; shadow.d = Ldir;
        Hit sh = sceneIntersect(shadow);
        bool visible = true;
        if(sh.hit){
            if(sh.t*sh.t < dist2 - 1e-4) visible = false;
        }
        if(visible){
            float nL = max(dot(h.n, Ldir), 0.0);
            if(nL>0.0){
                vec3 lightI = vec3(18.0,14.0,10.0) / max(0.001, dist2);
                accum += throughput * albedo * nL * lightI; // Lambert direct
            }
        }

        // decide next direction: if metal (rough small) use reflection, else diffuse
        if(h.mat == 2){
            // metal: reflection with roughness
            vec3 V = -ray.d;
            vec3 R = reflect(ray.d, h.n);
            // add roughness by random hemisphere around R
            vec3 rndDir = sampleHemisphere(h.n, seed);
            vec3 basisT = normalize(cross(abs(h.n.x) > 0.1 ? vec3(0,1,0) : vec3(1,0,0), h.n));
            vec3 basisB = cross(h.n, basisT);
            // mix R with rndDir based on roughness
            vec3 perturbed = normalize(mix(R, rndDir, rough));
            ray.o = h.p + h.n*0.001;
            ray.d = perturbed;
            // throughput multiply by F (approx using albedo as F0)
            float cosTheta = max(dot(perturbed, h.n), 0.0);
            vec3 F0 = albedo; // metal F0 approx by albedo
            vec3 F = fresnelSchlick(max(dot(V,h.n),0.0), F0);
            throughput *= F;
        } else {
            // diffuse: cosine-weighted hemisphere
            vec3 newDir = sampleHemisphere(h.n, seed);
            ray.o = h.p + h.n*0.001;
            ray.d = newDir;
            throughput *= albedo;
        }

        // Russian roulette (simple)
        if(bounce > 2){
            float p = max(0.05, max(throughput.r, max(throughput.g, throughput.b)));
            if(rnd(seed) > p) break;
            throughput /= p;
        }
    }

    // read previous accumulated color
    vec4 prev = imageLoad(accumImage, ivec2(pix));
    vec3 prevRGB = prev.rgb;
    // running average: new = (prev * frame + sample) / (frame+1)
    float f = float(uFrame);
    vec3 newRGB = (prevRGB * f + accum) / (f + 1.0);
    imageStore(accumImage, ivec2(pix), vec4(newRGB, 1.0));
}
)";


const char* bilateral_filter = R"(
#version 430
layout(local_size_x = 16, local_size_y = 16) in;

layout(rgba32f, binding = 0) uniform readonly image2D inputImage;
layout(rgba32f, binding = 1) uniform writeonly image2D outputImage;

uniform ivec2 iResolution;
uniform float sigma_s; // 공간 가중치
uniform float sigma_r; // 색상 가중치

void main(){
    ivec2 pix = ivec2(gl_GlobalInvocationID.xy);
    if(pix.x >= iResolution.x || pix.y >= iResolution.y) return;

    vec3 c = imageLoad(inputImage, pix).rgb;
    vec3 sum = vec3(0.0);
    float wsum = 0.0;

    int radius = int(2.0 * sigma_s);
    for(int y=-radius;y<=radius;y++){
        for(int x=-radius;x<=radius;x++){
            ivec2 neighbor = pix + ivec2(x,y);
            if(neighbor.x<0 || neighbor.y<0 || neighbor.x>=iResolution.x || neighbor.y>=iResolution.y) continue;

            vec3 cn = imageLoad(inputImage, neighbor).rgb;

            float gs = exp(-float(x*x + y*y)/(2.0*sigma_s*sigma_s));
            float gr = exp(-dot(cn-c,cn-c)/(2.0*sigma_r*sigma_r));
            float w = gs * gr;
            sum += cn * w;
            wsum += w;
        }
    }

    vec3 result = sum / max(wsum,0.0001);
    imageStore(outputImage, pix, vec4(result,1.0));
}
)";


// ---------- helper to create accum texture ----------
GLuint createAccumTexture(int w, int h){
    GLuint tex;
    glGenTextures(1,&tex);
    glBindTexture(GL_TEXTURE_2D, tex);
    glTexStorage2D(GL_TEXTURE_2D, 1, GL_RGBA32F, w, h);
    // initialize to 0
    std::vector<float> zero(w*h*4, 0.0f);
    glTexSubImage2D(GL_TEXTURE_2D, 0, 0,0, w,h, GL_RGBA, GL_FLOAT, zero.data());
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    return tex;
}

// ---------- GLFW key callback ----------
void keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods){
    if(action == GLFW_PRESS && key == GLFW_KEY_SPACE){
        bool* rotateBox = (bool*)glfwGetWindowUserPointer(window);
        *rotateBox = !(*rotateBox);
        std::cout << "Box rotation toggled: " << (*rotateBox ? "ON" : "OFF") << "\n";
    }
}

// ---------- main ----------
int main(){
    if(!glfwInit()){
        std::cerr<<"GLFW init failed\n"; return -1;
    }
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    GLFWwindow* win = glfwCreateWindow(WIN_W, WIN_H, "Progressive Path Tracer (compute)", nullptr, nullptr);
    if(!win){ std::cerr<<"create window failed\n"; return -1; }
    glfwMakeContextCurrent(win);

    if(!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)){
        std::cerr<<"glad load failed\n"; return -1;
    }
    glViewport(0,0,WIN_W,WIN_H);
    glDisable(GL_CULL_FACE);
    glEnable(GL_TEXTURE_2D);

    createFullscreenQuad();

    // create display program
    GLuint progDisp = createProgramFromSrc(vs_display, fs_display);
    // create compute
    GLuint progComp = createComputeFromSrc(cs_pathtracer);

    // accumulation texture
    int W = WIN_W, H = WIN_H;
    GLuint accumTex = createAccumTexture(W,H);

    // bind accumTex as image unit 0
    glBindImageTexture(0, accumTex, 0, GL_FALSE, 0, GL_READ_WRITE, GL_RGBA32F);

    // display texture uniform
    glUseProgram(progDisp);
    glUniform1i(glGetUniformLocation(progDisp, "accumTex"), 0);

    // camera params
    glm::vec3 camPos(0.0f, 0.4f, 3.2f);
    glm::vec3 camTarget(0.0f, 0.1f, 0.0f);
    glm::vec3 camDir = glm::normalize(camTarget - camPos);
    glm::vec3 camRight = glm::normalize(glm::cross(camDir, glm::vec3(0,1,0)));
    glm::vec3 camUp = glm::normalize(glm::cross(camRight, camDir));
    float fov = glm::radians(45.0f);

    bool rotateBox = true;
    glfwSetWindowUserPointer(win, &rotateBox);
    glfwSetKeyCallback(win, keyCallback);

    int frameCount = 0;
    double startTime = glfwGetTime();
    float prevBoxAngle = 0.0f;

    GLuint filteredTex = createAccumTexture(W,H); // accumTex와 동일한 포맷
    GLuint progBilateral = createComputeFromSrc(bilateral_filter);


    // main loop
    while(!glfwWindowShouldClose(win)){
        glfwPollEvents();

        // animate box rotation
        double t = glfwGetTime() - startTime;
        float boxAngle = prevBoxAngle;
        if (rotateBox) boxAngle = float(t * 0.8f); // radians, rotates slowly

        // reset accumulation if resolution changed or if scene moved significantly
        bool resetAccum = false;
        
        if(fabs(boxAngle - prevBoxAngle) > 1e-4){
            // box moved -> reset accumulation
            resetAccum = true;
        }
        prevBoxAngle = boxAngle;

        if(resetAccum){
            frameCount = 0;
            // clear accum texture to zero
            glBindTexture(GL_TEXTURE_2D, accumTex);
            std::vector<float> zero(W*H*4, 0.0f);
            glTexSubImage2D(GL_TEXTURE_2D, 0, 0,0, W,H, GL_RGBA, GL_FLOAT, zero.data());
            // ensure image binding updated
            glBindImageTexture(0, accumTex, 0, GL_FALSE, 0, GL_READ_WRITE, GL_RGBA32F);
        }
        // dispatch compute to add one sample per pixel
        glUseProgram(progComp);
        // uniforms
        glUniform2i(glGetUniformLocation(progComp, "iResolution"), W, H);
        glUniform1i(glGetUniformLocation(progComp, "uFrame"), frameCount);
        glUniform1f(glGetUniformLocation(progComp, "uTime"), float(t));
        // camera basis
        glm::vec3 forward = glm::normalize(glm::vec3(camTarget) - camPos);
        glm::vec3 right = glm::normalize(glm::cross(forward, glm::vec3(0,1,0)));
        glm::vec3 up = glm::normalize(glm::cross(right, forward));
        glUniform3f(glGetUniformLocation(progComp,"uCamPos"), camPos.x, camPos.y, camPos.z);
        glUniform3f(glGetUniformLocation(progComp,"uCamForward"), forward.x, forward.y, forward.z);
        glUniform3f(glGetUniformLocation(progComp,"uCamRight"), right.x, right.y, right.z);
        glUniform3f(glGetUniformLocation(progComp,"uCamUp"), up.x, up.y, up.z);
        glUniform1f(glGetUniformLocation(progComp,"uFov"), fov);
        glUniform1f(glGetUniformLocation(progComp,"uBoxAngle"), boxAngle);

        // ensure accum image binding is current
        glBindImageTexture(0, accumTex, 0, GL_FALSE, 0, GL_READ_WRITE, GL_RGBA32F);

        // dispatch
        int gx = (W + 15) / 16;
        int gy = (H + 15) / 16;
        glDispatchCompute(gx, gy, 1);
        glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);

        float frame_size = 2048;
        // -------- 멈췄을 때 bilateral filter ----------
        if(!rotateBox && frameCount > frame_size){
            glUseProgram(progBilateral);
            glUniform2i(glGetUniformLocation(progBilateral,"iResolution"), W,H);
            glUniform1f(glGetUniformLocation(progBilateral,"sigma_s"), 2.0f);
            glUniform1f(glGetUniformLocation(progBilateral,"sigma_r"), 0.1f);

            glBindImageTexture(0, accumTex, 0, GL_FALSE, 0, GL_READ_ONLY, GL_RGBA32F);
            glBindImageTexture(1, filteredTex, 0, GL_FALSE, 0, GL_WRITE_ONLY, GL_RGBA32F);

            glDispatchCompute(gx, gy, 1);
            glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);
            std::cerr<<"Finish";
        }

        // present accum texture to screen
        glClear(GL_COLOR_BUFFER_BIT);
        glUseProgram(progDisp);
        glActiveTexture(GL_TEXTURE0);

        if(!rotateBox && frameCount > frame_size)
            glBindTexture(GL_TEXTURE_2D, filteredTex); // 필터 적용된 텍스처 출력
        else
            glBindTexture(GL_TEXTURE_2D, accumTex);    // 일반 accum 출력
        glUniform1i(glGetUniformLocation(progDisp,"frameCount"), frameCount);
        glBindVertexArray(quadVAO);
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
        glBindVertexArray(0);

        glfwSwapBuffers(win);
        frameCount++;
    }

    glfwDestroyWindow(win);
    glfwTerminate();
    return 0;
}

