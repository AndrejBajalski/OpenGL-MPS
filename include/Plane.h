//
// Created by admin on 8/22/2025.
//

#ifndef PLANE_H
#define PLANE_H

#include <glm/gtc/type_ptr.hpp>
#include <vector>

struct Plane {
    glm::vec3 normal;
    float d;
    // Build plane from three points
    static Plane makePlane(const glm::vec3 &a, const glm::vec3 &b, const glm::vec3 &c);
    // Signed distance to plane
    static float pointPlaneDist(const glm::vec3 &p, const Plane &pl);

    // Check near wall
    static bool isOnPyramidWall(const glm::vec3 &p, float yBase, float yTop,
                         float xminBase, float xmaxBase,
                         float zminBase, float zmaxBase,
                         float epsilon);
};
#endif //PLANE_H
