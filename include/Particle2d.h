//
// Created by Andrej on 30/07/2025.
//

#ifndef PARTICLE2D_H
#define PARTICLE2D_H

#define PARTICLE_LIFETIME 2
#define MIN_HEAT 700

#include <glm/glm.hpp>

class Particle2d
{
public:
    glm::vec3 position;
    glm::vec3 velocity;
    glm::vec3 acceleration;
    float temperature;
    float lifetime;
    float mass;
    static float radius;
    ParticleType particleType;
    Particle2d(){
        position = glm::vec3(0.0f, 0.0f, 0.0f);
        velocity = glm::vec3(0.0f, 0.0f, 0.0f);
        acceleration = glm::vec3(0.0f, 0.0f, 0.0f);
        temperature = MIN_HEAT;
        lifetime = PARTICLE_LIFETIME;
        mass = 0.025f;
        particleType = ParticleType::FIRE;
    }
};
#endif //PARTICLE2D_H
