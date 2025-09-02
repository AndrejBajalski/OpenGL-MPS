//
// Created by Andrej on 29/07/2025.
//
#include <vector>
#include <glm/glm.hpp>

#include "Particle2d.h"
#include "PlaneGenerator.h"
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
    unsigned int VAO, VBO, positionVBO, temperatureVBO, texture1, UBO;
    Shader fireShader, objectShader;
    float dt;
    float PARTICLE_DISTANCE;
    glm::vec2 FIRE_LENGTH;
    static glm::vec3 offsetWorld;
    static bool shouldUpdateOffsets;
    PlaneGenerator fireBoundary;
    void initGlConfigurations();
    public:
    static int N_PARTICLES;
    static float P_RADIUS;
    PointParticleGenerator();
    PointParticleGenerator(float dt, Shader &shader, Shader &objectShader);
    PointParticleGenerator(float dt, Shader &shader, Shader &objectShader, float x_left, float x_right, float y_bottom, float y_top, float z_near, float z_far);
    void init();
    void update();
    void draw();
    void checkValid(Particle2d &p);
    void spawnParticle(Particle2d &p);
    void moveParticle(Particle2d &p, int index);
    float updateTemperature(Particle2d &p);
    void addNoise(Particle2d &p);
    void generatePointLights();
    bool isParticleNearObject(glm::vec3 object, float radius, glm::vec3 fireOffsets);
    void cleanup();
};
#endif //POINTPARTICLE_H
