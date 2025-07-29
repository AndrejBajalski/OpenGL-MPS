//
// Created by Andrej on 29/07/2025.
//
#include <vector>
#include <glm/glm.hpp>

#ifndef POINTPARTICLE_H
#define POINTPARTICLE_H
#define MIN_HEAT 700
#define MAX_HEAT 1500
#define PARTICLE_LIFETIME 2
#define MAX_PARTICLES 2000

struct Particle2d {
    glm::vec2 position;
    glm::vec2 velocity;
    float temperature;
    float lifetime;
    Particle2d(){
        position = glm::vec2(0.0f, 0.0f);
        velocity = glm::vec2(0.0f, 0.0f);
        temperature = MIN_HEAT;
        lifetime = PARTICLE_LIFETIME;
    }
};
class PointParticleGenerator
{
    std::vector<Particle2d> particles;
    unsigned int VAO, VBO;
    public:
    PointParticleGenerator();
    void init();
    void initGlConfigurations();
    void update();
    void draw();

};
#endif //POINTPARTICLE_H
