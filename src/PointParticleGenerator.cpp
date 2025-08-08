//
// Created by Andrej on 29/07/2025.
//
#include<PointParticleGenerator.h>
#include<ctime>
#include <random>
#include "emps.hpp"
#include "Camera.hpp"
#include "Camera.hpp"
#include "glad/glad.h"
#include "Particle2d.h"

#define FIRE_LEFT (-0.3f)
#define FIRE_RIGHT 0.3f
#define FIRE_BOTTOM (-0.5f)
#define FIRE_TOP (0.5f)
#define MAX_N_PARTICLES 4000
#define C_VIS 0.1f
#define C_BUO 0.5f
#define PARTICLE_RADIUS 0.005f
#define SCREEN_WIDTH 1200
#define SCREEN_HEIGHT 1000
#define AMBIENT_HEAT 35

std::vector<glm::vec3> position_offsets;
std::vector<glm::vec3> colors;
static const glm::vec3 FIRE_0 = {1.0f, 0.9098f, 0.03137f};
static const glm::vec3 FIRE_1 = {1.0f, 0.8078f, 0.0f};
static const glm::vec3 FIRE_2 = {1.0f, 0.6039f, 0.0f};
static const glm::vec3 FIRE_3 = {1.0f, 0.3529f, 0.0f};
static const float UPPER_BOUND = FIRE_TOP + 0.15f;
static int N_PARTICLES = 1000;
static std::default_random_engine generator;
static const float SPAWNING_OFFSET_X = PARTICLE_RADIUS*2*5;
static std::normal_distribution<float> distribution(0.0f, SPAWNING_OFFSET_X);
static float DT;
float calculateBuoyantForce(Particle2d &particle);
float calculateViscosityForce(Particle2d &p);
float calculatePressureForce(Particle2d &p);
float Particle2d::radius = 0.0f;

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
    int nX = (int)FIRE_LENGTH.x;
    int nY = (int)FIRE_LENGTH.y;
    int counter = 0;
    for (int i=0; i<nX; i++){
        for (int j=0; j<nY; j++){
            Particle2d particle = Particle2d();
            float x = FIRE_LEFT + i * (PARTICLE_DISTANCE + PARTICLE_RADIUS);
            float y = FIRE_BOTTOM + j * (PARTICLE_DISTANCE + PARTICLE_RADIUS);
            glm::vec3 offset = glm::vec3(x, y, 0.0f);
            particle.position += offset;
            position_offsets.push_back(offset);
            empsPtr->setPosition(counter, particle.position.x, particle.position.y, particle.position.z);
            if ((i>=nX/4 && i<=3*nX/4) && (j<nY/3)){
                particle.temperature =  MAX_HEAT;
                particle.particleType = ParticleType::FIRE;
                empsPtr->setParticleType(counter, particle.particleType);
            }else if ((i>=nX/5 && i<nX/4 || i>3*nX/4 && i<=4*nX/5) && (j<2*nY/3) || (i>=nX/4 && i<=3*nX/4) && (j>=nY/4 && j<2*nY/3)){
                particle.temperature = MIN_HEAT;
                particle.particleType = ParticleType::FIRE;
                empsPtr->setParticleType(counter, particle.particleType);
            }else {
                particle.temperature = AMBIENT_HEAT;
                particle.particleType = ParticleType::AIR;
                empsPtr->setParticleType(counter, particle.particleType);
            }
            this->particles.push_back(particle);
            glm::vec3 color = calculateColor(particle);
            colors.push_back(color);
            counter++;
        }
    }
    N_PARTICLES = counter;
    std::cout<<"TOTAL NUMBER OF PARTICLES: "<<N_PARTICLES<<std::endl;
    this->empsPtr->setNumberOfParticles(N_PARTICLES);
    //instanced variables
    generateInstanceBuffers(nX*nY, &this->positionVBO, &this->VAO, &position_offsets[0], 1);
    generateInstanceBuffers(nX*nY, &this->colorVBO, &this->VAO, &colors[0], 2);
    //calculate constants
    int middleParticleIndex = nX/2*nY - nY/2;
    this->empsPtr->calculateConstantParameter(middleParticleIndex);
}

void PointParticleGenerator::initGlConfigurations()
{
    float initialQuad[] = {
        //position
        PARTICLE_RADIUS, -PARTICLE_RADIUS, 0.0f,
        PARTICLE_RADIUS, PARTICLE_RADIUS, 0.0f,
        -PARTICLE_RADIUS, -PARTICLE_RADIUS, 0.0f,
        -PARTICLE_RADIUS, PARTICLE_RADIUS, 0.0f
    };
    //position
    glGenBuffers(1, &this->VBO);
    glGenVertexArrays(1, &this->VAO);
    glBindBuffer(GL_ARRAY_BUFFER, this->VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(initialQuad), &initialQuad[0], GL_STATIC_DRAW);
    glBindVertexArray(this->VAO);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3*sizeof(float), reinterpret_cast<void *>(0));
}

void PointParticleGenerator::update()
{
    double maxNi = 0.0;
    double minNi = 10.0;
    double maxPressure = 0.0;
    int maxNiIndex = 0, minNiIndex = 0;
    for (int i=0; i<particles.size(); i++)
    {
        Particle2d &p = this->particles[i];
        p.lifetime -= dt;
        // apply all the respective forces to the acceleration equation
        calculateBuoyantForce(p);
        moveParticle(p, i);
        // empsPtr->collision();
        double ni = empsPtr->calculateParticleNumberDensity(i);
        double pressure = empsPtr->calculatePressure_forExplicitMPS(i);
        empsPtr->calculatePressureGradient_forExplicitMPS(i);
        empsPtr->moveParticleUsingPressureGradient(particles[i], i);
        checkValid(p);
        // update particle position and color respectively
        position_offsets[i] = p.position;
        glm::vec3 resColor = calculateColor(p);
        colors[i] = resColor;
        if (ni>maxNi) {
            maxNi = ni;
            maxNiIndex = i;
        }
        if (ni<minNi) {
            minNi = ni;
            minNiIndex = i;
        }
        if (pressure>maxPressure) {
            maxPressure = pressure;
        }
    }
    // std::cout<<"Max Particle number density: Particle "<<maxNiIndex<<" -> "<<maxNi<<std::endl;
    // std::cout<<"Max Pressure: "<<maxPressure<<std::endl;
    updateBuffers(this->positionVBO, &position_offsets[0], this->particles.size());
    updateBuffers(this->colorVBO, &colors[0], this->particles.size());
}

void PointParticleGenerator::draw()
{
    glBindVertexArray(this->VAO);
    //for instanced drawing
    glDrawArraysInstanced(GL_TRIANGLE_STRIP, 0, 4, (int)this->particles.size());
    glBindVertexArray(0);
}

void PointParticleGenerator::generateInstanceBuffers(int nParticles, unsigned int *instanceVBO, unsigned int *VAO, glm::vec3 *arrayPointer, int index) {
    //instanceMatrix VBO
    glGenBuffers(1, instanceVBO);
    glBindBuffer(GL_ARRAY_BUFFER, *instanceVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec3) * nParticles, arrayPointer, GL_STATIC_DRAW);
    //instanceMatrix VAO
    glBindVertexArray(*VAO);
    glEnableVertexAttribArray(index);
    glVertexAttribPointer(index, 3, GL_FLOAT, GL_FALSE, sizeof(float)*3, reinterpret_cast<void *>(0));
    //attribute divisor
    glVertexAttribDivisor(index, 1);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
}

glm::vec3 PointParticleGenerator::calculateColor(Particle2d &p)
{
    if (p.temperature<MIN_HEAT)
        return {.0f, .0f, .91f};
    float r = 1.0f;
    float g = p.temperature/MAX_HEAT*FIRE_0[1];
    float b = p.temperature/MAX_HEAT*FIRE_0[2];
    return {r,g,b};
}

void PointParticleGenerator::updateBuffers(unsigned int instanceVBO, glm::vec3 *arrayPointer, int nParticles) {
    glBindBuffer(GL_ARRAY_BUFFER, instanceVBO);
    glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(glm::vec3) * nParticles, arrayPointer);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void PointParticleGenerator::spawnParticle(Particle2d &p) {
    ParticleType tmpType = p.particleType;
    p = Particle2d(tmpType);
    float tmp = distribution(generator);
    float x = std::max(-SPAWNING_OFFSET_X, std::min(tmp, SPAWNING_OFFSET_X));
    p.position.x = x;
    p.position.y = FIRE_BOTTOM;
}

void PointParticleGenerator::checkValid(Particle2d &p) {
    if (p.lifetime<=0.0f || p.position.y>FIRE_TOP || p.position.y<FIRE_BOTTOM || p.position.x<=FIRE_LEFT-0.15f || p.position.x>=FIRE_RIGHT+0.15f) {
        spawnParticle(p);
    }
}
float calculateBuoyantForce(Particle2d &p) {
    float T = (p.temperature - AMBIENT_HEAT)/AMBIENT_HEAT;
    float res = C_BUO * p.mass * 9.81 * T;
    p.acceleration.y += res;
    return res;
}
float calculateViscosityForce(Particle2d &p) {
    /**TODO
     *Calculate viscosity force using the EMPS method
     **/
    return 0.0f;
}
float calculatePressureForce(Particle2d &p) {
    /**TODO
     *Calculate pressure force using the EMPS method
     **/
    return 0.0f;
}

void PointParticleGenerator::moveParticle(Particle2d &p, int index) {
    p.velocity += p.acceleration * glm::vec3(dt);
    p.position += p.velocity * glm::vec3(dt);
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