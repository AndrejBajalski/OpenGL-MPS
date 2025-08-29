//
// Created by admin on 7/17/2025.
//

#ifndef PARTICLE_H
#define PARTICLE_H

#include <vector>
#include "ParticleType.h"

class Sphere {
public:
    unsigned int sphereVAO, sphereVBO, sphereEBO, texture;
    static std::vector<float> sphere_vertices;
    static std::vector<unsigned int> indices;
    static std::vector<int> lineIndices;
    static float radius;
    static int sectorCount, stackCount;

    Sphere();
    Sphere(int sectorCount, int stackCount, float radius);
    Sphere generateSphere();
    static void setRadius(double radius);
    static void setSectorAndStackCount(int sectorCount, int stackCount);
    void initGlConfig();
    void draw();
};

#endif //PARTICLE_H
