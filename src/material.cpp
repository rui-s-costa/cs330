#include "material.h"

Material::Material() {
    specular = glm::vec3(0.5f, 0.5f, 0.5f);
    shininess = 0.0f;
    useNormalMap = false;
}

void Material::setSpecular(glm::vec3 specular) {
    this->specular = specular;
}

void Material::setSpecular(float r, float g, float b) {
    setSpecular(glm::vec3(r, g, b));
}

void Material::setShininess(float shininess) {
    this->shininess = shininess;
}

