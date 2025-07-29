//
// Created by Andrej on 29/07/2025.
//
#include<PointParticleGenerator.h>
#include "glad/glad.h"

#define FIRE_LEFT (-0.5f)
#define FIRE_RIGHT 0.5f
#define FIRE_BOTTOM (-0.5f)
#define FIRE_TOP (0.5f)
#define DISTANCE 0.01f
#define N_PARTICLES 1000
#define PARTICLE_DISTANCE 0.01f
#define PARTICLE_DIAMETER 0.02f
#define DT 0.01

void PointParticleGenerator::init()
{
    initGlConfigurations();
    Particle2d particle = Particle2d();
    float ratio = 1.0f;
    int nX = (int)std::sqrt((N_PARTICLES/ratio));
    int nY = N_PARTICLES/nX;
    for (int i=0; i<nX; i++){
        for (int j=0; j<nY; j++){
            float x = FIRE_LEFT + i * DISTANCE;
            float y = FIRE_BOTTOM + j * DISTANCE;
            glm::vec2 offset = glm::vec2(x, y);
            particle.position += offset;
            this->particles.push_back(particle);
        }
    }
}
void PointParticleGenerator::initGlConfigurations()
{
    glm::vec3 initialPosition = glm::vec3(0.0f);
    glGenBuffers(1, &this->VBO);
    glGenVertexArrays(1, &this->VAO);
    glBindBuffer(GL_ARRAY_BUFFER, this->VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec3), &initialPosition, GL_STATIC_DRAW);
    glBindVertexArray(this->VAO);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 0, reinterpret_cast<void *>(0));
    glEnableVertexAttribArray(0);
    //for instanced drawing
    glVertexAttribDivisor(0, 1);
}
void PointParticleGenerator::update()
{
    for (int i=0; i<particles.size(); i++)
    {
        Particle2d &p = this->particles[i];
        p.lifetime -= DT; // reduce life
        if (p.lifetime > 0.0f)
        {	// particle is alive, thus update
            p.position -= p.velocity * glm::vec2(DT);
        }
    }
}

void PointParticleGenerator::draw()
{
    glBindVertexArray(this->VAO);
    //for instanced drawing
    glDrawArraysInstanced(GL_POINTS, 0, 2, (int)this->particles.size());
    glBindVertexArray(0);

}