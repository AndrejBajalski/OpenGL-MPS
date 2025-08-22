//
// Created by Andrej on 29/07/2025.
//
#include <chrono>
#include<PointParticleGenerator.h>
#include<ctime>
#include <random>
#include "emps.hpp"
#include "Camera.hpp"
#include "glad/glad.h"
#include "Particle2d.h"

#define STB_IMAGE_IMPLEMENTATION
#include <glm/gtc/type_ptr.hpp>
#include "stb_image.h"
#include "Plane.h"

#define FIRE_LEFT (-0.5f)
#define FIRE_RIGHT 0.5f
#define FIRE_BOTTOM (-1.0f)
#define FIRE_TOP (0.7f)
#define MAX_N_PARTICLES 2000
#define C_VIS 0.1f
#define C_BUO 0.36f
#define PARTICLE_RADIUS 0.05f
#define SCREEN_WIDTH 1200
#define SCREEN_HEIGHT 1000
#define AMBIENT_HEAT 30
#define MIN_HEAT 1500.0f
#define MAX_HEAT 3800.0f
#define SMOKE_HEAT 410.0f
#define GRAVITY 9.81f
#define MAX_LIGHT_POINTS 10
#define SIGMA 5.670374E-8
#define FIRE_BASE_ANGLE 80.0f

std::vector<glm::vec3> position_offsets;
std::vector<float> particleTemperatures;

struct PointLightWrapper {
    std::vector<glm::vec3> positions;
};
PointLightWrapper wrapper;
// static constexpr float SPAWNING_OFFSET_X = PARTICLE_RADIUS*2*2;
static constexpr float SPAWNING_OFFSET_Z = PARTICLE_RADIUS*2*2;
static std::normal_distribution<float> distributionNormal(0.0f, FIRE_RIGHT/2);
static std::mt19937 rng((unsigned)std::chrono::high_resolution_clock::now().time_since_epoch().count());
static float DT;
float PointParticleGenerator::P_RADIUS;
int PointParticleGenerator::N_PARTICLES;

template<typename V>
    void generateInstanceBuffers(int nParticles, unsigned int *VBO, unsigned int *VAO, V *arrayPointer, int index, std::string type);
template<typename V>
    void updateBuffers(unsigned int instanceVBO, V *arrayPointer, int nParticles, std::string type);
void generateUBO(unsigned int *UBO, Shader &fireShader);
void updateUBO(unsigned int *UBO, glm::vec3 *data);
void generateTextures(unsigned int *texture);
float calculateBuoyantForce(Particle2d &particle);
float Particle2d::radius = 0.0f;

float randRange(float a, float b) {
    std::uniform_real_distribution<float> dist(a, b);
    return dist(rng);
}
int randIntRange(int a, int b) {
    std::uniform_int_distribution<int> dist(a, b);
    return dist(rng);
}

PointParticleGenerator::PointParticleGenerator(double dt, Shader fireShader, Shader objectShader): dt(dt), fireShader(fireShader), objectShader(objectShader) {
    this->PARTICLE_DISTANCE = PARTICLE_RADIUS*2.0f;
    this->FIRE_LENGTH = {(FIRE_RIGHT-FIRE_LEFT)/(this->PARTICLE_DISTANCE+PARTICLE_RADIUS*2), (FIRE_TOP-FIRE_BOTTOM)/(this->PARTICLE_DISTANCE+PARTICLE_RADIUS*2)};
    P_RADIUS = PARTICLE_RADIUS;
}

void PointParticleGenerator::init(float delta_time)
{
    initGlConfigurations();
    generateTextures(&this->texture1);
    configEmps();
    Particle2d::radius = PARTICLE_RADIUS;
    int counter = 0, j = 0;
    for (int i=0; i<MAX_N_PARTICLES; i++){
        Particle2d particle = Particle2d();
        spawnParticle(particle);
        if (i%(MAX_PARTICLES/MAX_LIGHT_POINTS)==0) {
            particle.particleType = ParticleType::POINT_LIGHT;
            wrapper.positions.push_back(particle.position);
        }
        position_offsets.push_back(particle.position);
        particleTemperatures.push_back(particle.temperature);
        empsPtr->setPosition(counter, particle.position.x, particle.position.y, particle.position.z);
        empsPtr->setParticleType(counter, particle.particleType);
        this->particles.push_back(particle);
        counter++;
    }
    N_PARTICLES = counter;
    this->empsPtr->setNumberOfParticles(MAX_N_PARTICLES);
    //instanced variables
    generateInstanceBuffers(N_PARTICLES, &this->positionVBO, &this->VAO, &position_offsets[0], 2, "vec3");
    generateInstanceBuffers(N_PARTICLES, &this->temperatureVBO, &this->VAO, &particleTemperatures[0], 3, "float");
    generateUBO(&this->UBO, this->objectShader);
}

void PointParticleGenerator::initGlConfigurations()
{
    float initialQuad[] = {
        //position                                //UV coords
        PARTICLE_RADIUS, -PARTICLE_RADIUS, 0.0f,  1.0f, 0.0f,     //bottom right
        PARTICLE_RADIUS, PARTICLE_RADIUS, 0.0f,   1.0f, 1.0f,     //top right
        -PARTICLE_RADIUS, -PARTICLE_RADIUS, 0.0f, 0.0f, 0.0f,     //bottom left
        -PARTICLE_RADIUS, PARTICLE_RADIUS, 0.0f,  0.0f, 1.0f      //top left
    };
    //position
    glGenBuffers(1, &this->VBO);
    glGenVertexArrays(1, &this->VAO);
    glBindBuffer(GL_ARRAY_BUFFER, this->VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(initialQuad), &initialQuad[0], GL_STATIC_DRAW);
    glBindVertexArray(this->VAO);
    glEnableVertexAttribArray(0);
    //aPos
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5*sizeof(float), reinterpret_cast<void *>(0));
    //aUV
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5*sizeof(float), reinterpret_cast<void *>(3*sizeof(float)));
}

void PointParticleGenerator::update()
{
    for (int i=0; i<particles.size(); i++)
    {
        Particle2d &p = this->particles[i];
        p.lifetime -= dt;
        // apply all the respective forces to the acceleration equation
        calculateBuoyantForce(p);
        moveParticle(p, i);
        checkValid(p);
        addNoise(p);
        //update particle temperature
        updateTemperature(p);
        // update particle position and color respectively
        position_offsets[i] = p.position;
        particleTemperatures[i] = p.temperature;
    }
    // std::cout<<"Max Particle number density: Particle "<<maxNiIndex<<" -> "<<maxNi<<std::endl;
    // std::cout<<"Max Pressure: "<<maxPressure<<std::endl;
    updateBuffers(this->positionVBO, &position_offsets[0], (int)particles.size(), "vec3");
    updateBuffers(this->temperatureVBO, &particleTemperatures[0], (int)particles.size(), "float");
    updateUBO(&this->UBO, &wrapper.positions[0]);
}

void PointParticleGenerator::draw()
{
    glBindVertexArray(this->VAO);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, this->texture1);
    glBindVertexArray(VAO);
    //for instanced drawing of a quad
    // glDrawArraysInstanced(GL_TRIANGLE_STRIP, 0, 4, (int)this->particles.size())
    //for instanced drawing of a cube
    glDrawArraysInstanced(GL_TRIANGLES, 0, 36, (int) this->particles.size());;
    glBindVertexArray(0);
}

void PointParticleGenerator::spawnParticle(Particle2d &p) {
    ParticleType tmpType = p.particleType;
    p = Particle2d(tmpType);
    float tmp = distributionNormal(rng);
    float x = std::max(FIRE_LEFT, std::min(tmp, FIRE_RIGHT));
    float y = FIRE_BOTTOM;
    float z = randRange(-2*SPAWNING_OFFSET_Z, -SPAWNING_OFFSET_Z);
    float t = MAX_HEAT;
    float life = p.lifetime * randRange(0.65f, 1.0f);
    // p.velocity = glm::vec3(randRange(-0.4f, 0.4f), randRange(1.2f, 2.6f), randRange(-0.4f,0.4f));
    p.velocity = glm::vec3(0.0f);
    p.position.x = x;
    p.position.y = y;
    p.position.z = z;
    p.temperature = t;
    p.lifetime = life;
}

bool checkClippingVolume(Particle2d &p) {
    float t = (p.position.y - FIRE_BOTTOM)/(FIRE_TOP - FIRE_BOTTOM);
    float xmin = FIRE_LEFT * (1.0f - t) - 2*PARTICLE_RADIUS;
    float xmax = FIRE_RIGHT * (1.0f - t) + 2*PARTICLE_RADIUS;
    float zmin = -2*SPAWNING_OFFSET_Z * (1.0f - t) - 2*PARTICLE_RADIUS;
    float zmax = -SPAWNING_OFFSET_Z * (1.0f - t) + 2*PARTICLE_RADIUS;
    return (p.position.x >= xmin && p.position.x <= xmax &&
            p.position.z >= zmin && p.position.z <= zmax);
}

void PointParticleGenerator::checkValid(Particle2d &p) {
    if (p.lifetime < 0.0 || !checkClippingVolume(p)) {
        spawnParticle(p);
    }
}
float calculateBuoyantForce(Particle2d &p) {
    float T = C_BUO * (p.temperature - AMBIENT_HEAT)/AMBIENT_HEAT - GRAVITY;
    p.acceleration.y = T;
    return T;
}

float PointParticleGenerator::updateTemperature(Particle2d &p) {
    float t0;
    float area = PARTICLE_RADIUS*PARTICLE_RADIUS*6;
    float delta_x = fabs(p.position.x);
    // if (delta_x >= FIRE_RIGHT) {
    //     t0 = (MIN_HEAT - AMBIENT_HEAT)/2;
    //     p.particleType = ParticleType::OUTER_PARTICLE;
    // }
    if (Plane::isOnPyramidWall(p.position, FIRE_BOTTOM, FIRE_TOP, FIRE_LEFT, FIRE_RIGHT,
        -2*SPAWNING_OFFSET_Z, -SPAWNING_OFFSET_Z, PARTICLE_RADIUS)) {
        t0 = (MIN_HEAT - AMBIENT_HEAT)/2;
        p.particleType = ParticleType::OUTER_PARTICLE;
    }
    else {
        //linear gradient for temperature for inner particles
        t0 = p.temperature - (p.temperature+MIN_HEAT)/(2*FIRE_RIGHT) * delta_x * 0.33f;
        p.particleType = ParticleType::FIRE;
    }
    auto radiationGradient = static_cast<float>(SIGMA * (pow(p.temperature, 4)-pow(t0, 4)) * area * dt) ;
    p.temperature -= radiationGradient;
    if (p.temperature <= MIN_HEAT) {
        p.temperature = randRange(SMOKE_HEAT, MIN_HEAT);
    }
    return p.temperature;
}

void PointParticleGenerator::moveParticle(Particle2d &p, int index) {
    p.velocity += p.acceleration * glm::vec3(dt);
    glm::vec3 dp =  p.velocity * glm::vec3(dt);
    p.position += dp;
    int j = 0;
    if(p.particleType==ParticleType::POINT_LIGHT) {
        wrapper.positions[j++] = p.position;
    }
    empsPtr->setPosition(index, p.position.x, p.position.y, p.position.z);
}

void PointParticleGenerator::addNoise(Particle2d &p) {
    int i = randIntRange(0, MAX_PARTICLES);
    float x = randRange(-0.01f, 0.015);
    if (p.particleType == ParticleType::OUTER_PARTICLE) {
        // p.position.x += x;
    }
    // this->particles[i].position.x+=x;
    p.position.x += randRange(-0.008, 0.008f);
    p.position.z += randRange(-0.008, 0.008f);
}

void generateUBO(unsigned int *UBO, Shader &fireShader) {
    glGenBuffers(1, UBO);
    glBindBuffer(GL_UNIFORM_BUFFER, *UBO);
    glBufferData(GL_UNIFORM_BUFFER, sizeof(glm::vec4)*MAX_LIGHT_POINTS, &wrapper.positions[0], GL_STATIC_DRAW);
    unsigned int uniformBlockIndex = glGetUniformBlockIndex(fireShader.ID, "PointLights");
    glUniformBlockBinding(fireShader.ID, uniformBlockIndex, 0);
    glBindBufferBase(GL_UNIFORM_BUFFER, 0, *UBO);
    glBindBuffer(GL_UNIFORM_BUFFER, 0);
}


template<typename V>
void generateInstanceBuffers(int nParticles, unsigned int *instanceVBO, unsigned int *VAO, V *arrayPointer, int index, std::string type) {
    //instanceMatrix VBO
    glGenBuffers(1, instanceVBO);
    glBindBuffer(GL_ARRAY_BUFFER, *instanceVBO);
    if (type == "vec3")
        glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec3) * nParticles, arrayPointer, GL_STATIC_DRAW);
    else if (type == "float")
        glBufferData(GL_ARRAY_BUFFER, sizeof(float) * nParticles, arrayPointer, GL_STATIC_DRAW);
    //instanceMatrix VAO
    glBindVertexArray(*VAO);
    glEnableVertexAttribArray(index);
    if (type=="vec3")
        glVertexAttribPointer(index, 3, GL_FLOAT, GL_FALSE, sizeof(float)*3, reinterpret_cast<void *>(0));
    else if (type=="float")
        glVertexAttribPointer(index, 1, GL_FLOAT, GL_FALSE, sizeof(float), reinterpret_cast<void *>(0));
    //attribute divisor
    glVertexAttribDivisor(index, 1);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void updateUBO(unsigned int *UBO, glm::vec3 *data) {
    glBindBuffer(GL_UNIFORM_BUFFER, *UBO);
    glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(glm::vec4)*MAX_LIGHT_POINTS, data);
    glBindBuffer(GL_UNIFORM_BUFFER, 0);
}

template<typename V>
void updateBuffers(unsigned int instanceVBO, V *arrayPointer, int nParticles, std::string type) {
    glBindBuffer(GL_ARRAY_BUFFER, instanceVBO);
    if (type=="vec3")
        glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(glm::vec3) * nParticles, arrayPointer);
    else if (type=="float")
        glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(float) * nParticles, arrayPointer);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
}
// load and create a texture
void generateTextures(unsigned int *texture) {
    // texture 1
    // ---------
    glGenTextures(1, texture);
    glBindTexture(GL_TEXTURE_2D, *texture);
    // set the texture wrapping parameters
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);	// set texture wrapping to GL_REPEAT (default wrapping method)
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
    // set texture filtering parameters
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    // load image, create texture and generate mipmaps
    int width, height, nrChannels;
    stbi_set_flip_vertically_on_load(true); // tell stb_image.h to flip loaded texture's on the y-axis.
    int smokeTextureNumber = randIntRange(1, 4);
    unsigned char *data = stbi_load("../res/textures/smoke3.png", &width, &height, &nrChannels, 0);
    if (data)
    {
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);
    }
    else
    {
        std::cout << "Failed to load texture" << std::endl;
    }
    stbi_image_free(data);
}

void PointParticleGenerator::configEmps() {
    this->empsPtr = EmpsSingleton::getInstance(FIRE_TOP, FIRE_BOTTOM, FIRE_LEFT, FIRE_RIGHT, PARTICLE_DISTANCE, PARTICLE_RADIUS);
    this->empsPtr->DT = DT;
}

void PointParticleGenerator::cleanup()
{
    glDeleteBuffers(1, &this->VBO);
    glDeleteVertexArrays(1, &this->VAO);
}