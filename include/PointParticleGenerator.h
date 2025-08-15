//
// Created by Andrej on 29/07/2025.
//
#include <vector>
#include <glm/glm.hpp>

#include "emps.hpp"
#include "Particle2d.h"
#include "Shader.hpp"

#ifndef POINTPARTICLE_H
#define POINTPARTICLE_H
#define MIN_HEAT 700
#define MAX_HEAT 1500
#define AMBIENT_HEAT 30
#define MAX_PARTICLES 2000

class PointParticleGenerator
{
    std::vector<Particle2d> particles;
    unsigned int VAO, VBO, positionVBO, temperatureVBO, texture1;
    Shader shader;
    float dt;
    float PARTICLE_DISTANCE;
    glm::vec2 FIRE_LENGTH;
    EmpsSingleton *empsPtr;
    void initGlConfigurations();
    public:
    static int N_PARTICLES;
    static float P_RADIUS;
    PointParticleGenerator();
    PointParticleGenerator(double dt, Shader shader);
    void init(float delta_time);
    void update();
    void draw();
    void checkValid(Particle2d &p);
    void spawnParticle(Particle2d &p);
    void moveParticle(Particle2d &p, int index);
    float calculatePressureForce(Particle2d &p);
    void configEmps();
    void cleanup();
};
#endif //POINTPARTICLE_H
