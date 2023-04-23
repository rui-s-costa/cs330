#ifndef MATERIAL_H
#define MATERIAL_H

#include "texture.h"
#include "geometry.h"

class Material {
public:
    Material();

    void setSpecular(glm::vec3 specular);
    void setSpecular(float r, float g, float b);
    void setShininess(float shininess);

    Texture diffuse;
    Texture normal;
    glm::vec3 specular;
    float shininess;
    bool useNormalMap;
};

#endif
