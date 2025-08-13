//
// Created by Andrej on 30/07/2025.
//

#ifndef PARTICLE2D_H
#define PARTICLE2D_H

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
        temperature = 6000;
        lifetime = 1.0f;
        mass = 0.025f;
        particleType = ParticleType::FIRE;
    }
    Particle2d(ParticleType particleType):Particle2d(){
        this->particleType = particleType;
        switch (particleType) {
            case ParticleType::FIRE:
                this->temperature = 6000.0f;
                break;
            case ParticleType::AIR:
                this->temperature = 303.15f;
                break;
            default:
                this->temperature = 303.15f;
        }
    }

};
#endif //PARTICLE2D_H
