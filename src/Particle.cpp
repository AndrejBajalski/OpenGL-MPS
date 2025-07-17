//
// Created by admin on 7/17/2025.
//
#include<math.h>
#include <vector>
#include <Particle.hpp>
#define PI 3.14159265358979323846

int Particle::sectorCount;
int Particle::stackCount;
double Particle::radius;
double Particle::particleDistance;

Particle::Particle() {
    generateParticleAsSphere();
}

Particle::Particle(int sectorCount, int stackCount, double radius, double particleDistance) {
    Particle::sectorCount = sectorCount;
    Particle::stackCount = stackCount;
    Particle::radius = radius;
    Particle::particleDistance = particleDistance;
    generateParticleAsSphere();
}

Particle Particle::generateParticleAsSphere() {
    float x, y, z, xy;                              // vertex position
    float nx, ny, nz, lengthInv = 1.0f / radius;    // vertex normal
    float s, t;                                     // vertex texCoord
    float sectorStep = 2 * PI / sectorCount;
    float stackStep = PI / stackCount;
    float sectorAngle, stackAngle;
    for(int i = 0; i <= stackCount; ++i) {
        stackAngle = PI / 2 - i * stackStep;        // starting from pi/2 to -pi/2
        xy = radius * cosf(stackAngle);             // r * cos(u)
        z = radius * sinf(stackAngle);              // r * sin(u)
        // add (sectorCount+1) vertices per stack
        // first and last vertices have same position and normal, but different tex coords
        for(int j = 0; j <= sectorCount; ++j) {
            sectorAngle = j * sectorStep;           // starting from 0 to 2pi
            // vertex position (x, y, z)
            x = xy * cosf(sectorAngle);             // r * cos(u) * cos(v)
            y = xy * sinf(sectorAngle);             // r * cos(u) * sin(v)
            sphere_vertices.push_back(x);
            sphere_vertices.push_back(y);
            sphere_vertices.push_back(z);
            // normalized vertex normal (nx, ny, nz)
            nx = x * lengthInv;
            ny = y * lengthInv;
            nz = z * lengthInv;
            sphere_vertices.push_back(nx);
            sphere_vertices.push_back(ny);
            sphere_vertices.push_back(nz);
        }
    }
    //generate indices for EBO
    int k1, k2;
    for(int i = 0; i < stackCount; ++i)
    {
        k1 = i * (sectorCount + 1);     // beginning of current stack
        k2 = k1 + sectorCount + 1;      // beginning of next stack
        for(int j = 0; j < sectorCount; ++j, ++k1, ++k2)
        {
            // 2 triangles per sector excluding first and last stacks
            // k1 => k2 => k1+1
            if(i != 0){
                indices.push_back(k1);
                indices.push_back(k2);
                indices.push_back(k1 + 1);
            }
            // k1+1 => k2 => k2+1
            if(i != (stackCount-1))
            {
                indices.push_back(k1 + 1);
                indices.push_back(k2);
                indices.push_back(k2 + 1);
            }
            // store indices for lines
            // vertical lines for all stacks, k1 => k2
            lineIndices.push_back(k1);
            lineIndices.push_back(k2);
            if(i != 0)  // horizontal lines except 1st stack, k1 => k+1
            {
                lineIndices.push_back(k1);
                lineIndices.push_back(k1 + 1);
            }
        }
    }
    return *this;
}

void Particle::setParticleType(PARTICLE_TYPE type) {
    this->type = type;
}
void Particle::setRadius(double radius) {
    Particle::radius = radius;
}
void Particle::setSectorAndStackCount(int sectorCount, int stackCount) {
    Particle::sectorCount = sectorCount;
    Particle::stackCount = stackCount;
}
void Particle::setParticleDistance(double particleDistance) {
    Particle::particleDistance = particleDistance;
}
void Particle::setPosition(double x, double y, double z) {
    PositionX = x;
    PositionY = y;
    PositionZ = z;
}

void Particle::setVelocity(double vx, double vy, double vz) {
    VelocityX = vx;
    VelocityY = vy;
    VelocityZ = vz;
}

