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
};
class PlaneGenerator {
public:
    std::vector<Plane> planes;
    PlaneGenerator();
    PlaneGenerator(float yBase, float yTop, float xminBase, float xmaxBase, float zminBase, float zmaxBase);
    void generatePlanes(float yBase, float yTop, float xminBase, float xmaxBase, float zminBase, float zmaxBase);
    // Build plane from three points
    static Plane makePlane(const glm::vec3 &a, const glm::vec3 &b, const glm::vec3 &c);
    float pointPlaneDist(const glm::vec3 &p, const Plane &pl);
    bool isOnPyramidWall(const glm::vec3 &p, float epsilon);
    // Signed distance to plane
    static float pointPlaneDist(const glm::vec3 &p, const PlaneGenerator &pl);

    // Check near wall
};
#endif //PLANE_H
