#include "Plane.h"

#include <glm/gtc/type_ptr.hpp>
#include <vector>

// Build plane from three points
Plane Plane::makePlane(const glm::vec3 &a, const glm::vec3 &b, const glm::vec3 &c) {
    glm::vec3 n = glm::normalize(glm::cross(b - a, c - a));
    float d = -glm::dot(n, a);
    return {n, d};
}

// Signed distance to plane
float Plane::pointPlaneDist(const glm::vec3 &p, const Plane &pl) {
    return glm::dot(pl.normal, p) + pl.d;
}

// Check near wall
bool Plane::isOnPyramidWall(const glm::vec3 &p, float yBase, float yTop,
                     float xminBase, float xmaxBase,
                     float zminBase, float zmaxBase,
                     float epsilon)
{
    glm::vec3 A(0, yTop, 0);
    glm::vec3 B1(xminBase, yBase, zminBase);
    glm::vec3 B2(xmaxBase, yBase, zminBase);
    glm::vec3 B3(xmaxBase, yBase, zmaxBase);
    glm::vec3 B4(xminBase, yBase, zmaxBase);

    std::vector<Plane> planes = {
        makePlane(A, B1, B2),
        makePlane(A, B2, B3),
        // makePlane(A, B3, B4),
        makePlane(A, B4, B1)
    };

    for (auto &pl : planes) {
        float dist = fabs(pointPlaneDist(p, pl));
        if (dist < epsilon) {
            return true; // close to wall
        }
    }
    return false;
}
