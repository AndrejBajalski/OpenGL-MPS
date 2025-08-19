//
// Created by admin on 7/17/2025.
//
#include<math.h>
#include <vector>
#include <Sphere.hpp>
#include <ParticleType.h>
#include <glad/glad.h>
#define PI 3.14159265358979323846

int Sphere::sectorCount;
int Sphere::stackCount;
float Sphere::radius;
std::vector<float> Sphere::sphere_vertices;
std::vector<int> Sphere::lineIndices;
std::vector<unsigned int> Sphere::indices;

Sphere::Sphere() {
    generateSphere();
    initGlConfig();
}

Sphere::Sphere(int sectorCount, int stackCount, float radius) {
    Sphere::sectorCount = sectorCount;
    Sphere::stackCount = stackCount;
    Sphere::radius = radius;
    generateSphere();
    initGlConfig();
}

Sphere Sphere::generateSphere() {
    float x, y, z, xy;                              // vertex position
    float nx, ny, nz, lengthInv = 1.0f / radius;    // vertex normal
    float s, t;                                     // vertex texCoord
    float sectorStep = 2 * PI / sectorCount;
    float stackStep = PI / stackCount;
    float sectorAngle, stackAngle;
    for(int i = 0; i <= stackCount; ++i) {
        stackAngle = PI / 2 - i * stackStep;
        double r = radius;// starting from pi/2 to -pi/2
        xy = r * cosf(stackAngle);             // r * cos(u)
        z = r * sinf(stackAngle);              // r * sin(u)
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
void Sphere::setRadius(double radius) {
    Sphere::radius = radius;
}
void Sphere::setSectorAndStackCount(int sectorCount, int stackCount) {
    Sphere::sectorCount = sectorCount;
    Sphere::stackCount = stackCount;
}

void Sphere::initGlConfig() {
    glGenVertexArrays(1, &sphereVAO);
    glBindVertexArray(sphereVAO);
    glGenBuffers(1, &sphereVBO);
    glGenBuffers(1, &sphereEBO);
    //VBO
    glBindBuffer(GL_ARRAY_BUFFER, sphereVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(float) * sphere_vertices.size(), sphere_vertices.data(), GL_STATIC_DRAW);
    //VAO
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float),
                          reinterpret_cast<void *>(0));
    glEnableVertexAttribArray(0);
    //normal attribute
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float),
                          reinterpret_cast<void *>(3 * sizeof(float)));
    glEnableVertexAttribArray(1);
    //EBO
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, sphereEBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(int), indices.data(), GL_STATIC_DRAW);
}

void Sphere::draw() {
    glBindVertexArray(sphereVAO);
    glDrawElements(GL_TRIANGLES, static_cast<unsigned int>(indices.size()), GL_UNSIGNED_INT, nullptr);
    glBindVertexArray(0);
}
