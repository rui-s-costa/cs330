#ifndef GEOMETRY_H
#define GEOMETRY_H

// GLM Math Header inclusions:
#include <glm/glm.hpp>
#include <glm/gtx/transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/quaternion.hpp>

// Fun math :)
#define PI 3.14159265358979323846
#define RADIAN(x) (float)(((x) * PI / 180.0f))
#define DEGREE(x) (float)(((x) * 180.0f / PI))

glm::vec3 crossProduct(const glm::vec3 &v1, const glm::vec3 &v2);
glm::vec3 normalize(const glm::vec3 &v);
glm::vec3 polygonNormal(glm::vec3 vPolygon[]);

#endif //GEOMETRY_H
