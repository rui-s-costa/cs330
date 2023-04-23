#include "texture.h"

#define STB_IMAGE_IMPLEMENTATION

#include <stb_image.h>
#include <iostream>

using namespace std;

// Constructor for the Texture class, initializing default values for its member variables
Texture::Texture() {
    textureID = { nullptr };
    textureTarget = GL_TEXTURE_2D;
    totalTextures = 0;
    width = 800;
    height = 600;
    channels = 4;
    frameBuffer = 0;
    renderBuffer = 0;
    drawBuffers = { nullptr };
}

// Function to load a texture from a file using stb_image
// Returns 'true' if the texture was loaded successfully, 'false' otherwise
bool Texture::load(const char* filename, unsigned int totalTextures, GLenum textureTarget, GLfloat filterMin,
    GLfloat filterMag, bool clamp) {
    // Load the image using stb_image
    unsigned char* data = stbi_load(filename, &width, &height, &channels, 0);
    if (data) {
        // Flip the image vertically
        flipImageVertically(data, width, height, channels);
        // Create a texture with the loaded image data
        bool status = create(data, totalTextures, textureTarget, filterMin, filterMag, clamp, GL_NONE);

        // Free the image data
        stbi_image_free(data);

        return status;
    }
    return false;
}

// Function to create a texture with the given parameters
// Returns 'true' if the texture was created successfully, 'false' otherwise
bool Texture::create(unsigned char* data, unsigned int totalTextures, GLenum textureTarget, GLfloat filterMin,
    GLfloat filterMag, bool clamp, GLenum attachment) {
    // Set member variables based on the input parameters
    this->totalTextures = totalTextures;
    this->textureTarget = textureTarget;
    this->totalTextures = totalTextures;

    // Allocate memory for the texture ID array
    textureID = new GLuint[totalTextures];

    // Initialize the texture with the given parameters
    if (!init(&data, &filterMin, &filterMag, clamp)) {
        return false;
    }

    // Initialize render target if attachment is specified
    initRenderTarget(&attachment);

    // Generate mipmaps for the texture
    glGenerateMipmap(textureTarget);

    // Unbind the texture
    glBindTexture(textureTarget, 0);

    return true;
}


// Function to destroy textures and release resources
void Texture::destroy() {
    if (textureID) {
        glDeleteTextures(totalTextures, textureID);
        delete[] textureID;
    }
    if (frameBuffer) {
        glDeleteFramebuffers(1, &frameBuffer);
    }
    if (renderBuffer) {
        glDeleteRenderbuffers(1, &renderBuffer);
    }
    if (drawBuffers) {
        delete[] drawBuffers;
    }
}

// Function to bind a specific texture to the active texture unit
void Texture::bind(unsigned int texture) {
    glActiveTexture(GL_TEXTURE0 + texture);
    glBindTexture(textureTarget, textureID[texture]);
}

// Function to bind the normal map texture to the active texture unit
void Texture::bindNormalMap() {
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(textureTarget, textureID[0]);
}

// Function to bind the texture as a render target for rendering to texture
void Texture::bindAsRenderTarget() {
    glBindFramebuffer(GL_FRAMEBUFFER, frameBuffer);
    glViewport(0, 0, width, height);
}

// Function to initialize the texture with the given parameters
// Returns 'true' if the texture was initialized successfully, 'false' otherwise
bool Texture::init(unsigned char** data, GLfloat* filterMin, GLfloat* filterMag, bool clamp) {
    // Generate and bind texture
    glGenTextures(totalTextures, textureID);
    for (int i = 0; i < totalTextures; ++i) {
        glBindTexture(textureTarget, textureID[i]);

        // Set texture parameters based on input
        if (clamp) {
            // Clamping mode
            glTexParameteri(textureTarget, GL_TEXTURE_MIN_FILTER, filterMin[i]);
            glTexParameteri(textureTarget, GL_TEXTURE_MAG_FILTER, filterMag[i]);

            glTexParameteri(textureTarget, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
            glTexParameteri(textureTarget, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
            glTexParameteri(textureTarget, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
        }
        else {
            // Repeat mode
            glTexParameteri(textureTarget, GL_TEXTURE_WRAP_S, GL_REPEAT);
            glTexParameteri(textureTarget, GL_TEXTURE_WRAP_T, GL_REPEAT);

            glTexParameteri(textureTarget, GL_TEXTURE_MIN_FILTER, filterMin[i]);
            glTexParameteri(textureTarget, GL_TEXTURE_MAG_FILTER, filterMag[i]);
        }

        // Upload texture data based on the number of channels
        if (channels == 3) {
            glTexImage2D(textureTarget, 0, GL_SRGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data[i]);
        }
        else if (channels == 4) {
            glTexImage2D(textureTarget, 0, GL_SRGB_ALPHA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data[i]);
        }
        else {
            cout << "Not implemented to handle image with " << channels << " channels" << endl;
            return false;
        }
    }

    return true;
}

// Function to initialize the render target with the given attachment parameters
// Returns 'true' if the render target was initialized successfully, 'false' otherwise
bool Texture::initRenderTarget(GLenum* attachments) {
    // Check if the attachments array is empty
    if (attachments == nullptr) {
        return false;
    }

    // Allocate memory for draw buffers
    drawBuffers = new GLenum[totalTextures];
    bool hasDepth = false;

    // Loop through attachments to check for depth/stencil attachment and set draw buffers
    for (int i = 0; i < totalTextures; ++i) {
        if (attachments[i] == GL_DEPTH_ATTACHMENT || attachments[i] == GL_STENCIL_ATTACHMENT) {
            drawBuffers[i] = GL_NONE; // No color attachment for depth/stencil attachment
            hasDepth = true; // Flag indicating that depth/stencil attachment is present
        }
        drawBuffers[i] = attachments[i];
        if (attachments[i] == GL_NONE) {
            continue; // Skip this iteration if the attachment is none
        }

        // Generate and bind framebuffer if not created already
        if (frameBuffer == 0) {
            glGenFramebuffers(1, &frameBuffer);
            glBindFramebuffer(GL_FRAMEBUFFER, frameBuffer);
        }

        // Create a renderbuffer for depth if no depth/stencil attachment is present
        if (!hasDepth) {
            glGenRenderbuffers(1, &renderBuffer);
            glBindRenderbuffer(GL_RENDERBUFFER, renderBuffer);
            glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, width, height);
            glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, renderBuffer);
        }

        // Attach the texture to the framebuffer
        glFramebufferTexture2D(GL_FRAMEBUFFER, attachments[i], textureTarget, textureID[i], 0);
    }

    // If framebuffer is created, set draw buffers and check the framebuffer status
    if (frameBuffer != 0) {
        glDrawBuffers(totalTextures, drawBuffers); // Set draw buffers

        // Check if the framebuffer is complete and return the result
        if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
            printf("ERROR: FrameBuffer initialization failed!\n");
            assert(false);
            return false;
        }

        return true;
    }

    return false;
}

// Function to flip an image vertically, used for loading images with correct orientation
void flipImageVertically(unsigned char* image, int width, int height, int channels) {
    for (int j = 0; j < height / 2; ++j) {
        int index1 = j * width * channels; // Index for the top row
        int index2 = (height - 1 - j) * width * channels; // Index for the bottom row

        // Swap the top and bottom rows
        for (int i = width * channels; i > 0; --i) {
            unsigned char tmp = image[index1];
            image[index1] = image[index2];
            image[index2] = tmp;
            ++index1;
            ++index2;
        }
    }
}

