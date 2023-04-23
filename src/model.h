#ifndef MODEL_H
#define MODEL_H

#include "material.h"
#include "geometry.h"

// The Mesh class is responsible for storing OpenGL data related to a specific mesh.
// It manages the VAOs, VBOs, and other related buffers needed for rendering the mesh.
class Mesh {
public:
    Mesh();

    // Releases resources allocated by the Mesh object.
    void destroy();

    GLuint vao; // Vertex Array Object handle used to store the vertex attribute configuration.
    GLuint vbo; // Vertex Buffer Object handle for storing the mesh's vertex data.
    GLuint indexBuffer; // Index Buffer Object handle for storing indexed rendering data.
    GLuint uvBuffer; // Buffer handle for storing the mesh's UV (texture) coordinates.
    GLuint normalBuffer; // Buffer handle for storing the mesh's normal data (vertex normals).
    GLuint tangentBuffer; // Buffer handle for storing the mesh's tangent data.
    GLuint bitangentBuffer; // Buffer handle for storing the mesh's bitangent data.
    GLuint nIndices; // The number of indices in the mesh.
    GLuint nVertices; // The number of vertices in the mesh.
    GLuint totalUVs; // The number of UV coordinates in the mesh.
    GLuint totalNormals; // The number of vertex normals in the mesh.
    Material material; // The material properties associated with the mesh.
    bool textured; // Indicates whether the mesh has a texture or not.
};

// The Model class serves as the base class for 3D models. It contains a Mesh object
// and handles the model's transformation properties such as position, rotation, and scale.
class Model {
public:
    Model();

    // Initializes the model. Derived classes should override this method.
    virtual bool init() { return false; };

    // Releases resources allocated by the Model object.
    virtual void destroy();

    // Renders the model. Derived classes should implement this method.
    virtual void render() = 0;

    // Loads a texture from a file and associates it with the model.
    bool loadTexture(const char* filename);

    // Loads a normal map from a file and associates it with the model.
    // Currently, this is only supported for cube models.
    bool loadNormalMap(const char* filename);

    // Sets the model's position in world space.
    void setPosition(float x, float y, float z);

    // Sets the model's rotation around the x, y, and z axes in world space.
    void setRotation(float x, float y, float z);

    // Sets the model's scale along the x, y, and z axes in world space.
    void setScale(float x, float y, float z);

    // Returns the model's position in world space.
    glm::vec3 getPosition();

    // Returns the model's rotation around the x, y, and z axes in world space.
    glm::vec3 getRotation();

    // Returns the model's scale along the x, y, and z axes in world space.
    glm::vec3 getScale();

    // Checks if the model has a material associated with it.
    bool hasMaterial();

    // Returns a pointer to the model's material.
    Material* getMaterial();

protected:
    Mesh mesh; // The Mesh object associated with the model.
    glm::vec3 position; // The model's position in world space.
    glm::vec3 rotation; // The model's rotation around the x, y, and z axes in world space.
    glm::vec3 scale; // The model's scale along the x, y, and z axes in world space.
};

#endif //MODEL_H
