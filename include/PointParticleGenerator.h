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
    unsigned int VAO, VBO, colorVBO, positionVBO, colorVAO;
    Shader shader;
    void initGlConfigurations();
    public:
    PointParticleGenerator();
    PointParticleGenerator(Shader shader);
    void init();
    void update();
    void draw();
    void generateInstanceBuffers(int nParticles, unsigned int VBO, unsigned int VAO, glm::vec3 *arrayPointer, int index);
    glm::vec3 calculateColor(Particle2d &p);
    void updateBuffers(unsigned int instanceVBO, glm::vec3 *arrayPointer, int nParticles);
    void cleanup();
};
#endif //POINTPARTICLE_H
