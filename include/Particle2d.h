//
// Created by Andrej on 30/07/2025.
//

#ifndef PARTICLE2D_H
#define PARTICLE2D_H

#define PARTICLE_LIFETIME 2
#define MIN_HEAT 700

class Particle2d
{
public:
    glm::vec3 position;
    glm::vec3 velocity;
    float temperature;
    float lifetime;
    static float radius;
    Particle2d(){
        position = glm::vec3(0.0f, 0.0f, 0.0f);
        velocity = glm::vec3(0.0f, 1.0f, 0.0f);
        temperature = MIN_HEAT;
        lifetime = PARTICLE_LIFETIME;
    }
};
#endif //PARTICLE2D_H
