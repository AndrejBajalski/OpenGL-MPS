//
// Created by admin on 7/17/2025.
//

#ifndef PARTICLE_H
#define PARTICLE_H

#include <vector>
#include "ParticleType.h"

// enum PARTICLE_TYPE { GHOST=-1, GHOST_OR_DUMMY=-1, FLUID=0, WALL=2, DUMMY_WALL=3 };

class Particle {
public:
    static std::vector<float> sphere_vertices;
    static std::vector<int> indices;
    static std::vector<int> lineIndices;
    ParticleType type = ParticleType::FLUID;
    static float radius;
    static double particleDistance;
    static int sectorCount, stackCount;
    double AccelerationX;
    double AccelerationY;
    double AccelerationZ;
    int    ParticleType;
    double PositionX;
    double PositionY;
    double PositionZ;
    double VelocityX;
    double VelocityY;
    double VelocityZ;
    double Pressure;
    double ParticleNumberDensity;
    int    BoundaryCondition;
    double SourceTerm;
    double MinimumPressure;

    Particle();
    Particle(int sectorCount, int stackCount, float radius, double particleDistance);
    Particle generateParticleAsSphere();
    void setParticleType(enum ParticleType type);
    static void setRadius(double radius);
    static void setSectorAndStackCount(int sectorCount, int stackCount);
    static void setParticleDistance(double particleDistance);
    void setPosition(double x, double y, double z);
    void setVelocity(double vx, double vy, double vz);
};

#endif //PARTICLE_H
