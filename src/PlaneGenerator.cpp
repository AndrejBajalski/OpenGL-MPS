#include "PlaneGenerator.h"

#include <glm/gtc/type_ptr.hpp>
#include <vector>

PlaneGenerator::PlaneGenerator() {
}

PlaneGenerator::PlaneGenerator(float yBase, float yTop, float xminBase, float xmaxBase, float zminBase, float zmaxBase) {
    generatePlanes(yBase, yTop, xminBase, xmaxBase, zminBase, zmaxBase);
}

void PlaneGenerator::generatePlanes(float yBase, float yTop, float xminBase, float xmaxBase, float zminBase, float zmaxBase) {
    glm::vec3 A((xminBase+xmaxBase)/2, yTop, (zminBase+zmaxBase)/2);
    glm::vec3 B1(xminBase, yBase, zminBase);
    glm::vec3 B2(xmaxBase, yBase, zminBase);
    glm::vec3 B3(xmaxBase, yBase, zmaxBase);
    glm::vec3 B4(xminBase, yBase, zmaxBase);
    this->planes = {
        makePlane(A, B1, B2),
        makePlane(A, B3, B4),
        makePlane(A, B2, B3),
        makePlane(A, B4, B1)
    };
}

// Build plane from three points
Plane PlaneGenerator::makePlane(const glm::vec3 &a, const glm::vec3 &b, const glm::vec3 &c) {
    glm::vec3 n = glm::normalize(glm::cross(b - a, c - a));
    float d = -glm::dot(n, a);
    return {n, d};
}

// Signed distance to plane
float PlaneGenerator::pointPlaneDist(const glm::vec3 &p, const Plane &pl) {
    return abs(glm::dot(pl.normal, p) + pl.d);
}

// Check near wall
bool PlaneGenerator::isOnPyramidWall(const glm::vec3 &p, float epsilon)
{
    for (int i=0; i < 4; i++) {
        if (i==0 || i==1)
            continue;
        float dist = pointPlaneDist(p, planes[i]);
        if (dist < epsilon) {
            return true; // close to wall
        }
    }
    return false;
}
