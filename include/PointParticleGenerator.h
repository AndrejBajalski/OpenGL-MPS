//
// Created by Andrej on 29/07/2025.
//
#include <vector>
#include <glm/glm.hpp>
#include "Particle2d.h"
#include "Shader.hpp"

#ifndef POINTPARTICLE_H
#define POINTPARTICLE_H
#define MIN_HEAT 700
#define MAX_HEAT 1500
#define MAX_PARTICLES 2000


class PointParticleGenerator
{
    std::vector<Particle2d> particles;
    unsigned int VAO, VBO, colorVBO, positionVBO, positionVAO, colorVAO;
    Shader shader;
    float dt;
    void initGlConfigurations();
    public:
    PointParticleGenerator();
    PointParticleGenerator(double dt, Shader shader);
    void init();
    void update();
    void draw();
    void generateInstanceBuffers(int nParticles, unsigned int *VBO, unsigned int *VAO, glm::vec3 *arrayPointer, int index);
    glm::vec3 calculateColor(Particle2d &p);
    void updateBuffers(unsigned int instanceVBO, glm::vec3 *arrayPointer, int nParticles);
    void checkValid(Particle2d &p);
    void spawnParticle(Particle2d &p);
    void moveParticle(Particle2d &p);
    void cleanup();
};
#endif //POINTPARTICLE_H
