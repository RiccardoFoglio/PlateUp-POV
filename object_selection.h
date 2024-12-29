#ifndef OBJECT_SELECTION_H
#define OBJECT_SELECTION_H

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <iostream>

bool intersectRayAABB(const glm::vec3& rayOrigin, const glm::vec3& rayDirection, const glm::vec3& boxMin, const glm::vec3& boxMax, float& tMin, float& tMax);
bool rayIntersectsCube(const glm::vec3& rayOrigin, const glm::vec3& rayDirection, const glm::vec3& cubePosition, float cubeSize);
bool rayIntersectsCuboid(const glm::vec3& rayOrigin, const glm::vec3& rayDirection, const glm::vec3& cuboidPosition, const glm::vec3& cuboidSize);
#endif
