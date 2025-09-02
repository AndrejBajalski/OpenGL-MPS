//
// Created by Andrej on 29/07/2025.
//
#include <random>
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

class PointParticleGenerator
{
    std::vector<Particle2d> particles;
    unsigned int VAO, VBO, positionVBO, temperatureVBO, texture1, UBO;
    Shader fireShader, objectShader;
    float dt;
    float FIRE_LEFT, FIRE_RIGHT, FIRE_BOTTOM, FIRE_TOP, FIRE_Z_NEAR, FIRE_Z_FAR;
    bool initialized = false;
    float PARTICLE_DISTANCE;
    glm::vec2 FIRE_LENGTH;
    static glm::vec3 offsetWorld;
    static bool shouldUpdateOffsets;
    PlaneGenerator fireBoundary;
    std::normal_distribution<float> distributionNormal;
    void initGlConfigurations();
    public:
    int N_PARTICLES;
    static float P_RADIUS;
    PointParticleGenerator();
    PointParticleGenerator(float dt, Shader &shader, Shader &objectShader);
    PointParticleGenerator(float dt, Shader &shader, Shader &objectShader, float volumeRatio, float x_left, float x_right, float y_bottom, float y_top, float z_near, float z_far);
    void init();
    void update();
    void draw();
    void checkValid(Particle2d &p);
    void spawnParticle(Particle2d &p);
    void moveParticle(Particle2d &p, int index);
    bool checkClippingVolume(Particle2d &p);
    float updateTemperature(Particle2d &p);
    void addNoise(Particle2d &p);
    void generatePointLights();
    bool isParticleNearObject(glm::vec3 object, float radius, glm::vec3 fireOffsets);
    void cleanup();
    bool isInitialized() const {
        return this->initialized;
    }
    void setNParticles(int n) {
        this->N_PARTICLES = n;
    }
    void initParticleDistribution();
    float getVolume();
};
#endif //POINTPARTICLE_H
