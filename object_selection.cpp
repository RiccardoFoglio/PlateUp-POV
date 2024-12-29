#include "object_selection.h"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <iostream>


bool intersectRayAABB(
    const glm::vec3& rayOrigin,
    const glm::vec3& rayDirection,
    const glm::vec3& boxMin,
    const glm::vec3& boxMax,
    float& tMin, // Output: the closest intersection distance
    float& tMax  // Output: the farthest intersection distance
) {
    tMin = 0.0f; // Start with the ray's origin
    tMax = std::numeric_limits<float>::max();

    for (int i = 0; i < 3; i++) {
        if (fabs(rayDirection[i]) < 1e-8) { // Parallel ray
            if (rayOrigin[i] < boxMin[i] || rayOrigin[i] > boxMax[i]) {
                return false; // No intersection
            }
        }
        else {
            // Compute intersection distances for this axis
            float t1 = (boxMin[i] - rayOrigin[i]) / rayDirection[i];
            float t2 = (boxMax[i] - rayOrigin[i]) / rayDirection[i];

            // Ensure t1 is the minimum and t2 is the maximum
            if (t1 > t2) std::swap(t1, t2);

            // Update tMin and tMax to find the intersection interval
            tMin = std::max(tMin, t1);
            tMax = std::min(tMax, t2);

            // If the intervals do not overlap, no intersection
            if (tMin > tMax) return false;
        }
    }
    return true; // Intersection occurs
}

// Function to detect ray-cube intersection
bool rayIntersectsCube(
    const glm::vec3& rayOrigin,
    const glm::vec3& rayDirection,
    const glm::vec3& cubePosition,
    float cubeSize
) {
    glm::vec3 boxMin = cubePosition - glm::vec3(cubeSize / 2.0f);
    glm::vec3 boxMax = cubePosition + glm::vec3(cubeSize / 2.0f);
    float tMin, tMax;

    return intersectRayAABB(rayOrigin, rayDirection, boxMin, boxMax, tMin, tMax);
}

bool rayIntersectsCuboid(
    const glm::vec3& rayOrigin,
    const glm::vec3& rayDirection,
    const glm::vec3& cuboidPosition,
    const glm::vec3& cuboidSize // Width, height, length
) {
    // Compute the bounding box for the cuboid
    glm::vec3 boxMin = cuboidPosition - cuboidSize / 2.0f;
    glm::vec3 boxMax = cuboidPosition + cuboidSize / 2.0f;
    float tMin, tMax;

    return intersectRayAABB(rayOrigin, rayDirection, boxMin, boxMax, tMin, tMax);
}