
#include <iostream>
#include <cstdlib>
#include "main.h"
#include "cylinder.h"
#include "plane.h"
#include "cube.h"
#include "camera.h"
#include "light.h"
#include "skybox.h"
#include "torus.h"
#include "pyramid.h"

// Include the standard namespace for convenience
using namespace std;

// Define constants for the window title, width, and height
const char* const WINDOW_TITLE = "Final Project Rui";
const int WINDOW_WIDTH = 800;
const int WINDOW_HEIGHT = 600;

// Declare a GLFWwindow pointer for the main window
GLFWwindow* gWindow = nullptr;

// Declare a boolean variable to track if the window is focused
bool windowFocused = false;

// Declare a Camera pointer for the main camera
Camera* camera;

// Declare variables to store the last known cursor position and frame times
float lastX = WINDOW_WIDTH / 2.0f;
float lastY = WINDOW_HEIGHT / 2.0f;
float deltaTime = 0.0f;
float lastFrame = 0.0f;

// Declare a boolean variable to control screen mirroring
bool screenMirror = true;

// Declare pointers for the Shader objects used for lights and normal mapping
Shader* lights;
Shader* normalmap;

// Declare a DirectionalLight object for the sun
DirectionalLight sun;

// Declare a struct for the computer monitor components
struct ComputerMonitor {
    Cube* shell;
    Cube* screen;
    Cube* scene;
    Cube* stand;
    Cube* base;
};

// Declare a pointer for the desk object (a Plane)
Plane* desk;

// Declare instances of the Computer and ComputerMonitor structs
ComputerMonitor computerMonitor;

// Declare a pointer for the can object (a Cylinder)
Cylinder* can;

Cylinder* canTop;

Cylinder* leg1;
Cylinder* leg2;
Cylinder* leg3;
Cylinder* leg4;

Torus* ring;

Pyramid* pyramid;

// Declare a pointer for the framebuffer texture
Texture* frameBuffer;

SkyBox* skyBox;

/**
 * @brief Main function that sets up and renders a 3D scene in an OpenGL window.
 *
 * This function initializes the application with the given command line arguments,
 * creates camera, lights, shaders, and objects for the scene, sets up textures, materials,
 * and positions for the objects, and enters the main render loop. Inside the loop,
 * it processes user input, updates frame time, renders the scene to a framebuffer
 * and the window, polls for window events, and swaps buffers. After the loop,
 * it cleans up and destroys objects, shaders, and textures, and terminates GLFW.
 *
 * @param argc The number of command line arguments
 * @param argv An array of command line argument strings
 * @return int EXIT_SUCCESS if the application runs successfully; EXIT_FAILURE otherwise
 */
int main(int argc, char* argv[]) {
    // Initialize the application with the given arguments
    // If initialization fails, return an error
    if (!initialize(argc, argv, &gWindow)) {
        return EXIT_FAILURE;
    }

    // Create objects and shaders for the scene
    camera = new Camera(glm::vec3(0.0f, 0.0f, 3.0f));
    lights = new Shader("shader/lights.vs", "shader/lights.frag");
    normalmap = new Shader("shader/normalmap.vs", "shader/normalmap.frag");
    
    desk = new Plane(4.0f, 2.5f);
    
    computerMonitor.shell = new Cube(2.1f, 0.9f, 0.1f);
    computerMonitor.screen = new Cube(2.0f, 0.8f, 0.05f);
    computerMonitor.scene = new Cube(2.0f, 0.8f, 0.05f);
    computerMonitor.stand = new Cube(0.5f, 0.6f, 0.1f);
    computerMonitor.base = new Cube(1.0f, 0.1f, 0.4f);
    
    can = new Cylinder(0.2f, 0.2f, 0.6f);
    canTop = new Cylinder(0.2f, 0.2f, 0.01f);

    leg1 = new Cylinder(0.08f, 0.08f, 3.0f); // set desired radius for legs
    leg1->setRotation(100.0f, 0.0f, 0.0f);

    leg2 = new Cylinder(0.08f, 0.08f, 3.0f); // set desired radius for legs
    leg2->setRotation(100.0f, 0.0f, 0.0f);
    
    leg3 = new Cylinder(0.08f, 0.08f, 3.0f); // set desired radius for legs
    leg3->setRotation(80.0f, 0.0f, 0.0f);
    
    leg4 = new Cylinder(0.08f, 0.08f, 3.0f); // set desired radius for legs
    leg4->setRotation(80.0f, 0.0f, 0.0f);

    // Define the scaling factors
    float scaleX = 0.1f;
    float scaleY = 0.1f;
    float scaleZ = 0.1f;

    // Create a Torus object with smaller dimensions
    ring = new Torus(20, 20, 0.3f * scaleX, 0.15f * scaleY);
    
    // Pyramid
    float baseLength = 2.0f;
    float height = baseLength / 1.618f;
    pyramid = new Pyramid(baseLength, height);


    skyBox = new SkyBox();

    // Set up textures, materials, and positions for the scene objects
    frameBuffer = new Texture();
    frameBuffer->create(0, 1, GL_TEXTURE_2D, GL_NEAREST, GL_LINEAR, false, GL_COLOR_ATTACHMENT0);
    
    desk->init("images/desk_texture.jpg");
    
    computerMonitor.shell->init("images/computer_monitor_texture.jpg");
    computerMonitor.screen->initBuffer(frameBuffer);
    computerMonitor.stand->init("images/computer_monitor_texture.jpg");
    computerMonitor.base->init("images/computer_monitor_texture.jpg");
    
    can->init("images/cup_texture.png");
    canTop->init("images/can_top.png");
    
    desk->setPosition(0.0f, -2.0f, -5.0f);
    
    computerMonitor.shell->setPosition(0.0f, -0.6f, -5.0f);
    computerMonitor.screen->setPosition(0.0f, -0.6f, -4.94f);
    computerMonitor.scene->setPosition(0.0f, -0.6f, -4.94f);
    computerMonitor.stand->setPosition(0.0f, -1.5f, -5.2f);
    computerMonitor.base->setPosition(0.0f, -2.0f, -5.0f);

    leg1->init("images/desk_texture.jpg");
    leg2->init("images/desk_texture.jpg");
    leg3->init("images/desk_texture.jpg");
    leg4->init("images/desk_texture.jpg");

    ring->init("images/ring_texture.jpg");

    pyramid->init("images/pyramid_texture.jpg");
    
    can->setPosition(-2.8f, -1.6f - 0.05f, -4.2f);
    can->setRotation(90.0f, 90.0f, 0.0f);
    canTop->setPosition(-2.8f, -1.3f - 0.05f, -4.2f);
    canTop->setRotation(90.0f, 0.0f, 0.0f);
    
    computerMonitor.shell->getMaterial()->setShininess(32.0f);
    computerMonitor.stand->getMaterial()->setShininess(64.0f);
    computerMonitor.base->getMaterial()->setShininess(64.0f);
    desk->getMaterial()->setShininess(32.0f);
    can->getMaterial()->setShininess(15.0f);
    canTop->getMaterial()->setShininess(50.0f);

    leg1->setPosition(3.0f, -3.5f, -7.0f);  // back right corner
    leg2->setPosition(-3.0f, -3.5f, -7.00f);   // front right corner
    leg3->setPosition(-3.0f, -3.5f, -3.00f);  // back left corner
    leg4->setPosition(3.0f, -3.5f, -3.00f);    // back right corner

    ring->setPosition(1.6f, -1.8f - 0.158f, -4.0f);
    ring->setRotation(-90.0f, 0.0f, 0.0f);

    pyramid->setPosition(2.9f, -1.39f, -6.5f);

    // Init SkyBox:
    skyBox->init("images/skybox");

    // Initialize the OpenGL window background color and enable sRGB
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glEnable(GL_FRAMEBUFFER_SRGB);

    // Main render loop
    while (!glfwWindowShouldClose(gWindow)) {
        // Process user input
        processInput(gWindow);

        // Update frame time
        auto currentFrame = (float)glfwGetTime();
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        // Render the scene to a framebuffer
        frameBuffer->bindAsRenderTarget();
        renderScene();

        // Render the scene to the window
        bindWindowRenderTarget();
        renderScene();

        // Poll for window events and swap buffers
        glfwPollEvents();
        glfwSwapBuffers(gWindow);
    }

    // Clean up and destroy objects, shaders, and textures
    desk->destroy();
    delete desk;
    computerMonitor.shell->destroy();
    delete computerMonitor.shell;
    computerMonitor.screen->destroy();
    delete computerMonitor.screen;
    computerMonitor.scene->destroy();
    delete computerMonitor.scene;
    computerMonitor.stand->destroy();
    delete computerMonitor.stand;
    computerMonitor.base->destroy();
    delete computerMonitor.base;
    can->destroy();
    delete can;
    canTop->destroy();
    delete canTop;

    leg1->destroy();
    delete leg1;
    leg2->destroy();
    delete leg2;
    leg3->destroy();
    delete leg3;
    leg4->destroy();
    delete leg4;

    ring->destroy();
    delete ring;

    pyramid->destroy();
    delete pyramid;

    skyBox->destroy();
    delete skyBox;

    frameBuffer->destroy();
    delete frameBuffer;

    lights->destroy();
    delete lights;
    normalmap->destroy();
    delete normalmap;
    delete camera;

    // Terminate GLFW and exit the application
    glfwTerminate();
    exit(EXIT_SUCCESS);
}


// Function to initialize GLFW, GLEW, and create an OpenGL window
bool initialize(int argc, char* argv[], GLFWwindow** window) {
    // Initialize the GLFW library
    glfwInit();

    // Set the context version and profile depending on the platform
#ifdef __APPLE__
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#else
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 4);
#endif
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    // Create a new GLFW window
    *window = glfwCreateWindow(WINDOW_WIDTH, WINDOW_HEIGHT, WINDOW_TITLE, nullptr, nullptr);
    // If window creation fails, print an error and terminate GLFW
    if (*window == nullptr) {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return false;
    }

    // Make the created window the current OpenGL context
    glfwMakeContextCurrent(*window);

    // Set callback functions for window events
    glfwSetFramebufferSizeCallback(*window, resizeWindow);
    glfwSetCursorPosCallback(*window, mouseCallback);
    glfwSetScrollCallback(*window, scrollCallback);
    glfwSetWindowFocusCallback(*window, focusCallback);

    // Disable the cursor for the window
    glfwSetInputMode(*window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    // Enable experimental features and initialize the GLEW library
    glewExperimental = GL_TRUE;
    GLenum GlewInitResult = glewInit();

    // If GLEW initialization fails, print an error and return false
    if (GLEW_OK != GlewInitResult) {
        std::cerr << glewGetErrorString(GlewInitResult) << std::endl;
        return false;
    }

    // Print the current OpenGL version
    cout << "INFO: OpenGL Version: " << glGetString(GL_VERSION) << endl;

    // Return true if initialization is successful
    return true;
}

// Function to process user input from the keyboard
void processInput(GLFWwindow* window) {
    // Variable to track whether the 'P' key is pressed
    static bool p_pressed = false;

    // Close the window if the 'ESC' key is pressed
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
        glfwSetWindowShouldClose(window, true);
    }

    // Move the camera based on key presses
    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) {
        camera->ProcessKeyboard(FORWARD, deltaTime);
    }
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) {
        camera->ProcessKeyboard(BACKWARD, deltaTime);
    }
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) {
        camera->ProcessKeyboard(LEFT, deltaTime);
    }
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) {
        camera->ProcessKeyboard(RIGHT, deltaTime);
    }
    if (glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS) {
        camera->ProcessKeyboard(UP, deltaTime);
    }
    if (glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS) {
        camera->ProcessKeyboard(DOWN, deltaTime);
    }

    // Toggle camera running state when the 'Left Shift' key is pressed or released
    if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS) {
        camera->run = true;
    }
    else if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_RELEASE) {
        camera->run = false;
    }

    // Toggle camera orthographic projection when the 'P' key is pressed and released
    if (!p_pressed && glfwGetKey(window, GLFW_KEY_P) == GLFW_PRESS) {
        camera->toggleOrthographic();
        p_pressed = true;
    }
    else if (p_pressed && glfwGetKey(window, GLFW_KEY_P) == GLFW_RELEASE) {
        p_pressed = false;
    }

    // Toggle screen mirror state when the '1' and '2' keys are pressed
    if (glfwGetKey(window, GLFW_KEY_1) == GLFW_PRESS) {
        screenMirror = false;
    }
    if (glfwGetKey(window, GLFW_KEY_2) == GLFW_PRESS) {
        screenMirror = true;
    }
}


// Function to process mouse movement input
void mouseCallback(GLFWwindow* window, double xpos, double ypos) {
    // Check if the window is focused
    if (windowFocused) {
        windowFocused = false;
    }
    else {
        // Calculate mouse movement offsets and update the previous mouse position
        float offsetX = xpos - lastX;
        float offsetY = lastY - ypos;
        lastX = xpos;
        lastY = ypos;

        // Process the mouse movement for the camera
        camera->ProcessMouseMovement(offsetX, offsetY);
    }
}


void scrollCallback(GLFWwindow *window, double xoffset, double yoffset) {
    camera->ProcessMouseScroll((float) yoffset);
}


void resizeWindow(GLFWwindow *window, int width, int height) {
    glViewport(0, 0, width, height);
}


/**
 * @brief Renders the scene with all its objects and applies the appropriate shaders.
 *
 * This function sets the background color, clears the color and depth buffers, and toggles
 * depth testing and face culling settings. It then renders the objects in the scene using
 * their corresponding shaders. The computer monitor's screen or scene is rendered based on
 * the screen mirror state. Finally, it renders the rest of the objects in the scene with
 * their respective shaders.
 */
void renderScene() {
    // Set the background color and clear the color and depth buffers
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // Disable depth testing and face culling
    glDisable(GL_DEPTH_TEST);
    glDisable(GL_CULL_FACE);

    skyBox->render(camera);

    // Enable depth testing and face culling
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);

    // Render the objects in the scene using their corresponding shaders
    render(desk, lights);
    render(computerMonitor.shell, normalmap);

    // Render the computer monitor screen or scene based on the screen mirror state
    if (screenMirror) {
        render(computerMonitor.screen, lights);
    }
    else {
        render(computerMonitor.scene, lights);
    }

    // Render the rest of the objects in the scene with their respective shaders
    render(computerMonitor.stand, normalmap);
    render(computerMonitor.base, normalmap);
    render(can, lights);
    render(canTop, lights);
    render(leg1, lights);
    render(leg2, lights);
    render(leg3, lights);
    render(leg4, lights);

    render(ring, lights);

    render(pyramid, lights);
}



/**
 * @brief Renders a 3D model with the specified shader, applying material properties, lighting, and transformations.
 *
 * This function takes a pointer to a Model object and a pointer to a Shader object. It sets the shader program as
 * active and computes the model matrix (M) by combining scaling, rotation, and translation matrices. The function
 * then sets the model, view, and projection matrices in the shader. If the model has a material, the function sets
 * the material properties in the shader, including whether the material uses a normal map.
 *
 * The shader is also updated with the directional light properties, camera position, and light position. Finally,
 * the function calls the render() method on the model to draw it using the specified shader.
 *
 * @param model  A pointer to the Model object to be rendered.
 * @param shader A pointer to the Shader object to be used for rendering the model.
 */
void render(Model* model, Shader* shader) {
    // Use the specified shader program
    shader->use();

    // Compute the model matrix (M) by combining scaling, rotation, and translation matrices
    glm::mat4 scale, rotation, translation, m;
    glm::quat rotationQuat;
    scale = glm::scale(glm::vec3(model->getScale()));
    rotationQuat = glm::quat(model->getRotation());
    rotation = glm::toMat4(rotationQuat);
    translation = glm::translate(model->getPosition());
    m = translation * rotation * scale;

    // Set the model, view, and projection matrices in the shader
    shader->setMat4("model", m);
    shader->setMat4("view", camera->getView());
    shader->setMat4("projection", camera->getProjection());

    // Set the material properties in the shader if the model has a material
    Material* material = model->getMaterial();
    if (model->hasMaterial()) {
        shader->setInt("material.diffuse", 0);
        shader->setInt("material.normal", 1);
        shader->setVec3("material.specular", material->specular);
        shader->setFloat("material.shininess", material->shininess);
    }

    // Set the shader properties related to normal maps, lighting, and camera
    shader->setBool("material.hasNormalMap", material->useNormalMap);
    shader->setVec3("dirLight.direction", sun.direction);
    shader->setVec3("dirLight.ambient", sun.ambient);
    shader->setVec3("dirLight.diffuse", sun.diffuse);
    shader->setVec3("dirLight.specular", sun.specular);
    shader->setVec3("viewPos", camera->getPosition());
    shader->setVec3("lightPos", sun.direction);

    // Render the model using the specified shader
    model->render();
}


// Callback function for when the window gains or loses focus
void focusCallback(GLFWwindow* window, int focused) {
    // If the window gains focus
    if (focused) {
        windowFocused = true;
        // Set the cursor position to the last known position
        glfwSetCursorPos(window, lastX, lastY);
    }
    else { // If the window loses focus
        windowFocused = false;
    }
}

// Function to bind the window framebuffer as the render target
void bindWindowRenderTarget() {
    // Bind the default framebuffer (window) for rendering
    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
    // Set the viewport to the size of the window
    glViewport(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT);
}
