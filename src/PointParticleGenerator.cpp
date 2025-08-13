//
// Created by Andrej on 29/07/2025.
//
#include <chrono>
#include<PointParticleGenerator.h>
#include<ctime>
#include <random>
#include "emps.hpp"
#include "Camera.hpp"
#include "Camera.hpp"
#include "glad/glad.h"
#include "Particle2d.h"

#define FIRE_LEFT (-0.2f)
#define FIRE_RIGHT 0.2f
#define FIRE_BOTTOM (-0.5f)
#define FIRE_TOP (0.5f)
#define MAX_N_PARTICLES 3000
#define C_VIS 0.1f
#define C_BUO 1.5f
#define PARTICLE_RADIUS 0.013f
#define SCREEN_WIDTH 1200
#define SCREEN_HEIGHT 1000
#define AMBIENT_HEAT 30
#define MIN_HEAT 1500.0f
#define MAX_HEAT 6000.0f
#define GRAVITY 9.81f

std::vector<glm::vec3> position_offsets;
std::vector<float> particleTemperatures;

static const glm::vec3 FIRE_0 = {1.0f, 0.9098f, 0.03137f};
static const glm::vec3 FIRE_1 = {1.0f, 0.8078f, 0.0f};
static const glm::vec3 FIRE_2 = {1.0f, 0.6039f, 0.0f};
static const glm::vec3 FIRE_3 = {1.0f, 0.3529f, 0.0f};
static const float UPPER_BOUND = FIRE_TOP + 0.15f;
static int N_PARTICLES = 1000;
static std::default_random_engine generator;
static const float SPAWNING_OFFSET_X = PARTICLE_RADIUS*2*2;
static const float SPAWNING_OFFSET_Z = PARTICLE_RADIUS*2*2;
static std::normal_distribution<float> distributionNormal(0.0f, FIRE_RIGHT/2);
static std::mt19937 rng((unsigned)std::chrono::high_resolution_clock::now().time_since_epoch().count());
static float DT;

template<typename V>
    void generateInstanceBuffers(int nParticles, unsigned int *VBO, unsigned int *VAO, V *arrayPointer, int index, std::string type);
template<typename V>
    void updateBuffers(unsigned int instanceVBO, V *arrayPointer, int nParticles, std::string type);
float calculateBuoyantForce(Particle2d &particle);
float updateTemperature(Particle2d &p);
float calculatePressureForce(Particle2d &p);
float Particle2d::radius = 0.0f;

float randRange(float a, float b) {
    std::uniform_real_distribution<float> dist(a, b);
    return dist(rng);
}

PointParticleGenerator::PointParticleGenerator(double dt, Shader shader): dt(dt), shader(shader) {
    this->PARTICLE_DISTANCE = PARTICLE_RADIUS*2.0f;
    this->FIRE_LENGTH = {(FIRE_RIGHT-FIRE_LEFT)/(this->PARTICLE_DISTANCE+PARTICLE_RADIUS*2), (FIRE_TOP-FIRE_BOTTOM)/(this->PARTICLE_DISTANCE+PARTICLE_RADIUS*2)};
}

void PointParticleGenerator::init(float delta_time)
{
    initGlConfigurations();
    DT = delta_time;
    configEmps();
    Particle2d::radius = PARTICLE_RADIUS;
    int counter = 0;
    for (int i=0; i<MAX_N_PARTICLES; i++){
        Particle2d particle = Particle2d();
        spawnParticle(particle);
        position_offsets.push_back(particle.position);
        particleTemperatures.push_back(particle.temperature);
        empsPtr->setPosition(counter, particle.position.x, particle.position.y, particle.position.z);
        empsPtr->setParticleType(counter, particle.particleType);
        this->particles.push_back(particle);
        counter++;
    }
    N_PARTICLES = counter;
    std::cout<<"TOTAL NUMBER OF PARTICLES: "<<N_PARTICLES<<std::endl;
    this->empsPtr->setNumberOfParticles(N_PARTICLES);
    //instanced variables
    generateInstanceBuffers(N_PARTICLES, &this->positionVBO, &this->VAO, &position_offsets[0], 2, "vec3");
    generateInstanceBuffers(N_PARTICLES, &this->temperatureVBO, &this->temperatureVAO, &particleTemperatures[0], 3, "float");
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
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5*sizeof(float), reinterpret_cast<void *>(0));
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5*sizeof(float), reinterpret_cast<void *>(3*sizeof(float)));
}

void PointParticleGenerator::update()
{
    double maxNi = 0.0;
    double minNi = 10.0;
    double maxPressure = 0.0;
    for (int i=0; i<particles.size(); i++)
    {
        Particle2d &p = this->particles[i];
        p.lifetime -= dt;
        // apply all the respective forces to the acceleration equation
        calculateBuoyantForce(p);
        moveParticle(p, i);
        checkValid(p);
        //update particle temperature
        updateTemperature(p);
        // update particle position and color respectively
        position_offsets[i] = p.position;
        particleTemperatures[i] = p.temperature;
    }
    // std::cout<<"Max Particle number density: Particle "<<maxNiIndex<<" -> "<<maxNi<<std::endl;
    // std::cout<<"Max Pressure: "<<maxPressure<<std::endl;
    updateBuffers(this->positionVBO, &position_offsets[0], this->particles.size(), "vec3");
    updateBuffers(this->temperatureVBO, &particleTemperatures[0], this->particles.size(), "float");
}

void PointParticleGenerator::draw()
{
    glBindVertexArray(this->VAO);
    //for instanced drawing
    glDrawArraysInstanced(GL_TRIANGLE_STRIP, 0, 4, (int)this->particles.size());
    glBindVertexArray(0);
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

template<typename V>
void updateBuffers(unsigned int instanceVBO, V *arrayPointer, int nParticles, std::string type) {
    glBindBuffer(GL_ARRAY_BUFFER, instanceVBO);
    if (type=="vec3")
        glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(glm::vec3) * nParticles, arrayPointer);
    else if (type=="float")
        glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(float) * nParticles, arrayPointer);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void PointParticleGenerator::spawnParticle(Particle2d &p) {
    ParticleType tmpType = p.particleType;
    p = Particle2d(tmpType);
    float tmp = distributionNormal(generator);
    float x = std::max(FIRE_LEFT, std::min(tmp, FIRE_RIGHT));
    float y = randRange(FIRE_BOTTOM, FIRE_BOTTOM+4*PARTICLE_RADIUS);
    float z = randRange(-SPAWNING_OFFSET_Z, SPAWNING_OFFSET_Z);
    float t = MAX_HEAT;
    float life = p.lifetime * randRange(0.65f, 1.0f);
    p.velocity = glm::vec3(randRange(-0.4f, 0.4f), randRange(1.2f, 2.6f), randRange(-0.4f,0.4f));
    p.position.x = x;
    p.position.y = y;
    p.position.z = z;
    p.temperature = t;
    p.lifetime = life;
}

void PointParticleGenerator::checkValid(Particle2d &p) {
    // if (p.lifetime<=0.0f || p.position.y>FIRE_TOP || p.position.y<FIRE_BOTTOM || p.position.x<=FIRE_LEFT-0.15f || p.position.x>=FIRE_RIGHT+0.15f) {
    //     spawnParticle(p);
    // }
    if (p.lifetime < 0.0) {
        spawnParticle(p);
    }
}
float calculateBuoyantForce(Particle2d &p) {
    float T = C_BUO * (p.temperature - AMBIENT_HEAT)/AMBIENT_HEAT - GRAVITY;
    return T;
}
float updateTemperature(Particle2d &p) {
    p.temperature -= 100.0f;
    if (p.temperature < MIN_HEAT)
        p.temperature = MIN_HEAT;
    return p.temperature;
}

float calculatePressureForce(Particle2d &p) {
    /**TODO
     *Calculate pressure force using the EMPS method
     **/
    return 0.0f;
}

void PointParticleGenerator::moveParticle(Particle2d &p, int index) {
    p.velocity += p.acceleration * glm::vec3(dt);
    glm::vec3 dp =  p.velocity * glm::vec3(dt);
    p.position += dp;
    empsPtr->setPosition(index, p.position.x, p.position.y, p.position.z);
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