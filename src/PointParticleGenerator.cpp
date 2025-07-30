//
// Created by Andrej on 29/07/2025.
//
#include<PointParticleGenerator.h>

#include "Camera.hpp"
#include "Camera.hpp"
#include "glad/glad.h"
#include "Particle2d.h"

#define FIRE_LEFT (-0.5f)
#define FIRE_RIGHT 0.5f
#define FIRE_BOTTOM (-0.5f)
#define FIRE_TOP (0.5f)
#define DISTANCE 0.01f
#define N_PARTICLES 1000
#define PARTICLE_DISTANCE 0.01f
#define DT 0.01
#define C_VIS 0.1f
#define PARTICLE_RADIUS 0.01f

std::vector<glm::vec3> position_offsets;
std::vector<glm::vec3> colors;

float Particle2d::radius = 0.0f;

PointParticleGenerator::PointParticleGenerator(Shader shader): shader(shader){}

void PointParticleGenerator::init()
{
    initGlConfigurations();
    Particle2d::radius = PARTICLE_RADIUS;
    shader.use();
    float ratio = 1.0f;
    int nX = (int)std::sqrt((N_PARTICLES/ratio));
    int nY = N_PARTICLES/nX;
    for (int i=0; i<nX; i++){
        for (int j=0; j<nY; j++){
            Particle2d particle = Particle2d();
            float x = FIRE_LEFT + i * DISTANCE;
            float y = FIRE_BOTTOM + j * DISTANCE;
            glm::vec3 offset = glm::vec3(x, y, 0.0f);
            particle.position += offset;
            position_offsets.push_back(offset);
            if (j<nY/3){
                particle.temperature =  MAX_HEAT;
            }else {
                particle.temperature = MIN_HEAT;
            }
            this->particles.push_back(particle);
            colors.push_back(calculateColor(particle));
        }
    }
    //instanced variables
    generateInstanceBuffers(nX*nY, this->positionVBO, this->VAO, &position_offsets[0], 1);
    generateInstanceBuffers(nX*nY, this->colorVBO, this->colorVAO, &colors[0], 2);
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
    for (int i=0; i<particles.size(); i++)
    {
        Particle2d &p = this->particles[i];
        p.lifetime -= DT; // reduce life
        // if (p.lifetime > 0.0f)
        // {	// particle is alive, thus update
            p.position += p.velocity * glm::vec3(DT);
            position_offsets[i] = p.position;
        // }
        glm::vec3 resColor = calculateColor(p);
        this->shader.use();
        colors.push_back(resColor);
    }
    updateBuffers(this->colorVBO, &colors[0], this->particles.size());
}

void PointParticleGenerator::draw()
{
    glBlendFunc(GL_SRC_ALPHA, GL_ONE);
    shader.use();
    glBindVertexArray(this->VAO);
    //for instanced drawing
    glDrawArraysInstanced(GL_TRIANGLE_STRIP, 0, 4, (int)this->particles.size());
    glBindVertexArray(0);
}

void PointParticleGenerator::generateInstanceBuffers(int nParticles, unsigned int instanceVBO, unsigned int VAO, glm::vec3 *arrayPointer, int index) {
    //instanceMatrix VBO
    glGenBuffers(1, &instanceVBO);
    glBindBuffer(GL_ARRAY_BUFFER, instanceVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec3) * nParticles, arrayPointer, GL_STATIC_DRAW);
    //instanceMatrix VAO
    glBindVertexArray(VAO);
    glEnableVertexAttribArray(index);
    glVertexAttribPointer(index, 3, GL_FLOAT, GL_FALSE, sizeof(float)*3, reinterpret_cast<void *>(0));
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    //attribute divisor
    glVertexAttribDivisor(index, 1);
}

glm::vec3 PointParticleGenerator::calculateColor(Particle2d &p)
{
    float r = .5f;
    float g = (3 - p.temperature/MAX_HEAT)/3;
    float b = (3 - p.temperature/MAX_HEAT)/3;
    return {r,g,b};
}

void PointParticleGenerator::updateBuffers(unsigned int instanceVBO, glm::vec3 *arrayPointer, int nParticles) {
    glBindBuffer(GL_ARRAY_BUFFER, instanceVBO);
    glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(glm::vec3) * nParticles, arrayPointer);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void PointParticleGenerator::cleanup()
{
    glDeleteBuffers(1, &this->VBO);
    glDeleteVertexArrays(1, &this->VAO);
}