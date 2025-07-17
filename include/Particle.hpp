//
// Created by admin on 7/17/2025.
//

#ifndef PARTICLE_H
#define PARTICLE_H

#include <vector>

enum PARTICLE_TYPE { GHOST=-1, GHOST_OR_DUMMY=-1, FLUID=0, WALL=2, DUMMY_WALL=3 };

class Particle {
public:
    std::vector<double> sphere_vertices;
    std::vector<int> indices;
    std::vector<int> lineIndices;
    PARTICLE_TYPE type = FLUID;
    static double radius;
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
    Particle(int sectorCount, int stackCount, double radius, double particleDistance);
    Particle generateParticleAsSphere();
    void setParticleType(PARTICLE_TYPE type);
    static void setRadius(double radius);
    static void setSectorAndStackCount(int sectorCount, int stackCount);
    static void setParticleDistance(double particleDistance);
    void setPosition(double x, double y, double z);
    void setVelocity(double vx, double vy, double vz);
};

#endif //PARTICLE_H
