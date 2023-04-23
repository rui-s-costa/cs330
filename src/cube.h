#ifndef CUBE_H
#define CUBE_H

#include "model.h"

class Cube : public Model {
public:
    Cube(float width = 0.5f, float height = 0.5f, float length = 0.5f);

    bool init(const char *filename);

    bool initBuffer(Texture *texture);

    void render() override;

private:
    float width;
    float height;
    float length;
    Texture *bufferTexture;
    bool renderToTexture;
};

#endif //CUBE_H
