#include "camera.h"

const float SPEED = 2.5f;
const float SENSITIVITY = 0.1f;
const float ZOOM = 45.0f;

// Default constructor for the Camera class
Camera::Camera() {
    Camera(glm::vec3(0.0f, 0.0f, 0.0f));
}

// Camera constructor that initializes the camera with a position, up vector, yaw, and pitch.
Camera::Camera(glm::vec3 position, glm::vec3 up, float yaw, float pitch) : Front(glm::vec3(0.0f, 0.0f, -1.0f)),
MovementSpeed(SPEED),
MouseSensitivity(SENSITIVITY),
Zoom(ZOOM) {
    Position = position;
    WorldUp = up;
    Yaw = yaw;
    Pitch = pitch;
    updateCameraVectors();
    run = false;
    runSpeed = MovementSpeed * 2;
    near = 0.1f;
    far = 100.0f;
    aspect = (4.0f / 3.0f); // 4/3 standard, 16/9 widescreen (window width / window height)
    ortho = false; // ortho off by default
}

// Camera constructor that takes scalar values for position, up vector, yaw, and pitch.
Camera::Camera(float posX, float posY, float posZ, float upX, float upY, float upZ, float yaw, float pitch) : Front(
    glm::vec3(0.0f, 0.0f, -1.0f)), MovementSpeed(SPEED), MouseSensitivity(SENSITIVITY), Zoom(ZOOM) {
    Camera(glm::vec3(posX, posY, posZ), glm::vec3(upX, upY, upZ), yaw, pitch);
}

// Computes the view matrix based on the camera's current position, front, and up vectors.
glm::mat4 Camera::getView() {
    return glm::lookAt(Position, Position + Front, Up);
}

// Computes the projection matrix based on the camera's zoom, aspect ratio, near, and far clipping planes.
glm::mat4 Camera::getProjection() {
    if (ortho) {
        float scale = 150;
        float width = 800;
        float height = 600;

        return glm::ortho(-(width / scale), (width / scale), -(height / scale), (height / scale), 0.1f,
            100.0f); // Orthographic projection
    }
    else {
        return glm::perspective(glm::radians(Zoom), aspect, near, far); // Perspective projection
    }
}

// Returns the camera's current position.
glm::vec3 Camera::getPosition() {
    return Position;
}

// Processes keyboard input for camera movement. The input direction is specified using the Camera_Movement enum.
void Camera::ProcessKeyboard(Camera_Movement direction, float deltaTime) {
    float velocity;

    if (run) {
        velocity = runSpeed * deltaTime;
    }
    else {
        velocity = MovementSpeed * deltaTime;
    }

    if (direction == FORWARD) {
        Position += Front * velocity;
    }
    if (direction == BACKWARD) {
        Position -= Front * velocity;
    }
    if (direction == LEFT) {
        Position -= Right * velocity;
    }
    if (direction == RIGHT) {
        Position += Right * velocity;
    }
    if (direction == UP) {
        Position += Up * velocity;
    }
    if (direction == DOWN) {
        Position -= Up * velocity;
    }
}



// Process input received from a mouse input system. Expects the offset value in both the x and y direction:
void Camera::ProcessMouseMovement(float xoffset, float yoffset, GLboolean constrainPitch) {
    xoffset *= MouseSensitivity;
    yoffset *= MouseSensitivity;

    Yaw += xoffset;
    Pitch += yoffset;

    // Make sure that when pitch is out of bounds, screen doesn't get flipped:
    if (constrainPitch) {
        if (Pitch > 89.0f) {
            Pitch = 89.0f;
        }
        if (Pitch < -89.0f) {
            Pitch = -89.0f;
        }
    }

    updateCameraVectors(); // update front, right, and up vectors using the updated euler angles
}

// Process input received from a mouse scroll-wheel event. Only requires input on the vertical wheel-axis:
void Camera::ProcessMouseScroll(float yoffset) {
    // For this assignment, the mouse scroll adjusts speed rather than zooming in:
    MovementSpeed -= yoffset;
    if (MovementSpeed < 1.0f) {
        MovementSpeed = 1.0f;
    }
    if (MovementSpeed > 50.0f) {
        MovementSpeed = 50.0f;
    }

}

// Calculates the front vector from the camera's updated euler angles:
void Camera::updateCameraVectors() {
    // Calculate the new Front vector
    glm::vec3 front;
    front.x = cos(glm::radians(Yaw)) * cos(glm::radians(Pitch));
    front.y = sin(glm::radians(Pitch));
    front.z = sin(glm::radians(Yaw)) * cos(glm::radians(Pitch));
    Front = glm::normalize(front);

    // Calculate the right and up vectors, normalizing them, because their length gets closer to 0 the more you look up
    // or down which results in slower movement:
    Right = glm::normalize(glm::cross(Front, WorldUp));
    Up = glm::normalize(glm::cross(Right, Front));
}

void Camera::toggleOrthographic() {
    ortho = !ortho;
}

