
#include "torus.h"

// Torus constructor
Torus::Torus(int mainSegments, int tubeSegments, float mainRadius, float tubeRadius) {
    vertices = nullptr;
    normals = nullptr;
    uvs = nullptr;
    indices = nullptr;
    _mainSegments = mainSegments;
    _tubeSegments = tubeSegments;
    _mainRadius = mainRadius;
    _tubeRadius = tubeRadius;
}

// Initialize the torus and load the texture
bool Torus::init(const char *filename) {
    if (!loadTexture(filename)) {
        return false;
    }

    build();

    // Create and bind vertex array object (VAO)
    glGenVertexArrays(1, &mesh.vao);
    glBindVertexArray(mesh.vao);

    // Initialize vertex buffer object (VBO) for vertex data
    glGenBuffers(1, &mesh.vbo);
    glBindBuffer(GL_ARRAY_BUFFER, mesh.vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec3) * mesh.nVertices, vertices, GL_STATIC_DRAW);

    // Set vertex attribute pointer and enable it
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void *) nullptr);
    glEnableVertexAttribArray(0);

    // Initialize index buffer object (IBO) for index data
    glGenBuffers(1, &mesh.indexBuffer);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mesh.indexBuffer);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(GLuint) * mesh.nIndices, indices, GL_STATIC_DRAW);

    // Initialize normal buffer object (NBO) for normal data
    glGenBuffers(1, &mesh.normalBuffer);
    glBindBuffer(GL_ARRAY_BUFFER, mesh.normalBuffer);
    glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec3) * mesh.nVertices, normals, GL_STATIC_DRAW);

    // Set normal attribute pointer and enable it
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, (void *) nullptr);
    glEnableVertexAttribArray(1);

    // Initialize UV buffer object (UBO) for texture coordinate data
    glGenBuffers(1, &mesh.uvBuffer);
    glBindBuffer(GL_ARRAY_BUFFER, mesh.uvBuffer);
    glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec2) * mesh.nVertices, uvs, GL_STATIC_DRAW);

    // Set UV attribute pointer and enable it
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 0, (void *) nullptr);
    glEnableVertexAttribArray(2);

    return true;
}

// Clean up the torus data
void Torus::destroy() {
    if (vertices != nullptr) {
        delete[] vertices;
    }
    if (normals != nullptr) {
        delete[] normals;
    }
    if (uvs != nullptr) {
        delete[] uvs;
    }
    if (indices != nullptr) {
        delete[] indices;
    }
}

// Render the torus
void Torus::render() {
    glEnableVertexAttribArray(0);
    glEnableVertexAttribArray(1);
    glEnableVertexAttribArray(2);

    if (mesh.textured) {
        mesh.material.diffuse.bind();
    }

    // Bind VAO and draw elements using indices
    glBindVertexArray(mesh.vao);
    glEnable(GL_PRIMITIVE_RESTART);
    glPrimitiveRestartIndex(_primitiveRestartIndex);
    glDrawElements(GL_TRIANGLE_STRIP, _numIndices, GL_UNSIGNED_INT, 0);
    glDisable(GL_PRIMITIVE_RESTART);
    glBindVertexArray(0);

    // Disable vertex attributes
    glDisableVertexAttribArray(0);
    glDisableVertexAttribArray(1);
    glDisableVertexAttribArray(2);
}

// This function builds a torus mesh by calculating the necessary
// vertices, texture coordinates, normals, and indices.

void Torus::build() {
    
    // Initialize variables for current angles and current face index
    float currentMainSegmentAngle = 0.0f;
    int index = 0;

    // Calculate and cache the total number of vertices and indices
    mesh.nVertices = (_mainSegments + 1) * (_tubeSegments + 1);
    _primitiveRestartIndex = mesh.nVertices;
    _numIndices = (_mainSegments * 2 * (_tubeSegments + 1)) + _mainSegments - 1;
    mesh.nIndices = _numIndices;

    // Allocate memory for vertices, normals, texture coordinates, and indices
    vertices = new glm::vec3[mesh.nVertices];
    normals = new glm::vec3[mesh.nVertices];
    uvs = new glm::vec2[mesh.nVertices];
    indices = new GLuint[_numIndices];

    // Calculate the step size for angles of main and tube segments
    auto mainSegmentAngleStep = glm::radians(360.0f / float(_mainSegments));
    auto tubeSegmentAngleStep = glm::radians(360.0f / float(_tubeSegments));

    // Calculate vertex positions for each segment and store them in vertices array
    for (auto i = 0; i <= _mainSegments; i++) {
        auto sinMainSegment = sin(currentMainSegmentAngle);
        auto cosMainSegment = cos(currentMainSegmentAngle);
        auto currentTubeSegmentAngle = 0.0f;
        for (auto j = 0; j <= _tubeSegments; j++) {
            auto sinTubeSegment = sin(currentTubeSegmentAngle);
            auto cosTubeSegment = cos(currentTubeSegmentAngle);

            // Calculate vertex position on the surface of torus
            vertices[index] = glm::vec3(
                    (_mainRadius + _tubeRadius * cosTubeSegment) * cosMainSegment,
                    (_mainRadius + _tubeRadius * cosTubeSegment) * sinMainSegment,
                    _tubeRadius * sinTubeSegment);
            ++index;

            // Update current tube angle
            currentTubeSegmentAngle += tubeSegmentAngleStep;
        }

        // Update main segment angle
        currentMainSegmentAngle += mainSegmentAngleStep;
    }

    // Calculate texture coordinates for each vertex and store them in uvs array
    auto mainSegmentTextureStep = 2.0f / float(_mainSegments);
    auto tubeSegmentTextureStep = 1.0f / float(_tubeSegments);
    index = 0;
    auto currentMainSegmentTexCoordV = 0.0f;
    for (auto i = 0; i <= _mainSegments; i++) {
        auto currentTubeSegmentTexCoordU = 0.0f;
        for (auto j = 0; j <= _tubeSegments; j++) {
            uvs[index] = glm::vec2(currentTubeSegmentTexCoordU, currentMainSegmentTexCoordV);
            ++index;

            currentTubeSegmentTexCoordU += tubeSegmentTextureStep;
        }

        // Update texture coordinate of main segment
        currentMainSegmentTexCoordV += mainSegmentTextureStep;
    }

    // Calculate normals for each vertex and store them in normals array
    index = 0;
    currentMainSegmentAngle = 0.0f;
    for (auto i = 0; i <= _mainSegments; i++) {
        auto sinMainSegment = sin(currentMainSegmentAngle);
        auto cosMainSegment = cos(currentMainSegmentAngle);
        auto currentTubeSegmentAngle = 0.0f;
        for (auto j = 0; j <= _tubeSegments; j++) {
            // Calculate sine and cosine of tube segment angle
            auto sinTubeSegment = sin(currentTubeSegmentAngle);
            auto cosTubeSegment = cos(currentTubeSegmentAngle);

            normals[index] = glm::vec3(cosMainSegment * cosTubeSegment, sinMainSegment * cosTubeSegment,
                                       sinTubeSegment);
            ++index;

            // Update current tube angle
            currentTubeSegmentAngle += tubeSegmentAngleStep;
        }

        // Update main segment angle
        currentMainSegmentAngle += mainSegmentAngleStep;
    }

    // Indices:
    index = 0; // reset again
    GLuint currentVertexOffset = 0;
    for (auto i = 0; i < _mainSegments; i++) {
        for (auto j = 0; j <= _tubeSegments; j++) {
            GLuint vertexIndexA = currentVertexOffset;
            indices[index] = vertexIndexA;
            ++index;

            GLuint vertexIndexB = currentVertexOffset + _tubeSegments + 1;
            indices[index] = vertexIndexB;
            ++index;

            currentVertexOffset++;
        }

        if (i != _mainSegments - 1) {
            indices[index] = _primitiveRestartIndex;
            ++index;
        }
    }
}
