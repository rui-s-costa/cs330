#ifndef PLANE_H
#define PLANE_H

#include "model.h"

class Plane : public Model {
public:
    explicit Plane(float width = 5.0f, float length = 5.0f);

    bool init(const char *filename);

    void render() override;

private:
    float width;
    float length;
};

#endif //PLANE_H
