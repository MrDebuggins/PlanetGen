/********************************
 * Code source: learnopengl.com *
 *******************************/
#pragma once

#include <chrono>

#include <GL/glew.h>
#include <GL/freeglut_std.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>


#ifndef PI
#define PI 3.141592
#endif

/**
 * \brief Defines several possible options for camera movement.
 * Used as abstraction to stay away from window-system specific input methods
 */
enum camera_movement {
    FORWARD = 'w',
    BACKWARD = 's',
    LEFT = 'a',
    RIGHT = 'd',
    UP = 'r',
    DOWN = 'f',
    Q = 'q',
    E = 'e'
};

// Default camera values
constexpr float yaw_c = -90.0f;
constexpr float pitch_c = 0.0f;
constexpr float speed_c = 1.0f;
constexpr float sensitivity_c = 0.1f;
constexpr float zoom_c = 1.0f;
const float coef_M = 1000000.0f;

/**
 * \brief An abstract camera class that processes input and calculates the corresponding Euler Angles,
 * Vectors and Matrices for use in OpenGL
 */
class Camera
{
public:
    int w = 700;
    int h = 400;

    // projection matrix
    glm::mat4x4 projectionMatrix;

    //
    float fovX, fovY = PI / 4;

    // camera Attributes
    glm::vec3 position_m = glm::vec3(0.0f); // meters
    glm::vec3 position_M = glm::vec3(0.0f); // mega meters
    glm::vec3 front;
    glm::vec3 up;
    glm::vec3 right;
    glm::vec3 worldUp;

    // euler Angles
    float yawE;
    float pitchE;

    // camera options
    float movementSpeed;
    float mouseSensitivity;
    float zoom;

    // mouse cursor
    float cursorX = 350;
    float cursorY = 200;

    // camera buttons states ( w a s d r f)
    bool buttons[6]{ false,false,false,false,false,false };
    
    /**
     * \brief Constructor with vectors and default options.
     * \param position camera position
     * \param up normalized camera orientation
     * \param yaw camera yaw
     * \param pitch camera pitch
     */
    Camera(glm::vec3 pos_m = glm::vec3(4799299.0f, 4799299.0f, 4799299.0f), glm::vec3 up = glm::vec3(0.0, 1.0, 0.0), float yaw = yaw_c, float pitch = pitch_c) : front(glm::vec3(0.0f, 0.0f, -1.0f)), movementSpeed(speed_c), mouseSensitivity(sensitivity_c), zoom(zoom_c)
    {
        updatePosition(pos_m);
        worldUp = up;
        this->yawE = yaw;
        this->pitchE = pitch;
        updateCameraVectors();
    }

    /**
     * \brief Returns the view matrix calculated using Euler Angles and the LookAt Matrix
     * \return view matrix
     */
    glm::mat4 getViewMatrix() const
    {
        return glm::lookAt(glm::vec3(0.0, 0.0, 0.0), front, up);
    }

    /**
     * \brief Return max between two field of view values (most cases is X)
     * \return field of view in radians
     */
    float getMaxFieldOfView()
    {
        return glm::max(fovX, fovY)*0.8f;
    }

    /**
     * \brief Calculate new projection matrix with new zoom and/or aspect ratio
     */
    void updateProjectionMatrix()
    {
        float aspectRatio = float(w) / float(h);
        fovY = PI / (4 * zoom);
        fovX = 2 * glm::atan(glm::tan(fovY * 0.5f) * aspectRatio);
        projectionMatrix = glm::perspective(fovY, aspectRatio, 0.1f, 10000000000.0f);
    }
    
    /**
     * \brief Processes input received from any keyboard input system.
     * Accepts input parameter in the form of camera defined ENUM (to abstract it from windowing systems)
     * \param direction key pressed (w,a,s,d keys correspond to camera_movement enum members)
     * \param state true if button pressed, false otherwise
     */
    void processKeyboard(const camera_movement direction, bool state)
    {
        if (direction == FORWARD)
            buttons[0] = state;
        if (direction == BACKWARD)
            buttons[1] = state;
        if (direction == LEFT)
            buttons[2] = state;
        if (direction == RIGHT)
            buttons[3] = state;
        if (direction == UP)
            buttons[4] = state;
        if (direction == DOWN)
            buttons[5] = state;
        if (direction == Q)
            movementSpeed /= 2;
        if (direction == E)
            movementSpeed *= 2;
    }

    /**
     * \brief Update camera position
     * \param dTime time passed since last update
     */
    void update(const float dTime)
    {
        const float velocity = movementSpeed * dTime;

        if (buttons[0])
            updatePosition(front * velocity);
        if (buttons[1])
            updatePosition(-front * velocity);
        if (buttons[2])
            updatePosition(-right * velocity);
        if (buttons[3])
            updatePosition(right * velocity);
        if (buttons[4])
            updatePosition(up * velocity);
        if (buttons[5])
            updatePosition(-up * velocity);
    }

    /**
     * \brief Processes input received from a mouse input system. Expects the offset value in both the x and y direction
     * \param x offset on x axis 
     * \param y offset on y axis
     * \param constrain_pitch constraint movement on y axis
     */
    void processMouseMovement(float x, float y, const GLboolean constrain_pitch = false)
    {
        x -= static_cast<float>(w) / 2.0f;
        y -= static_cast<float>(h) / 2.0f;

        yawE += x * mouseSensitivity / zoom;
        pitchE += -y * mouseSensitivity / zoom;

        cursorX = x;
    	cursorY = y;

        // make sure that when pitch is out of bounds, screen doesn't get flipped
        if (constrain_pitch)
        {
            if (pitchE > 89.0f)
                pitchE = 89.0f;
            if (pitchE < -89.0f)
                pitchE = -89.0f;
        }

        // update Front, Right and Up Vectors using the updated Euler angles
        updateCameraVectors();
    	glutWarpPointer(w / 2, h / 2);
    }
    
    /**
     * \brief Processes input received from a mouse scroll-wheel event. Only requires input on the vertical wheel-axis
     * \param y_offset wheel offset
     */
    void processMouseScroll(const float y_offset)
    {
        zoom += y_offset;
        if (zoom < 1.0f)
            zoom = 1.0f;
        if (zoom > 45.0f)
            zoom = 45.0f;

        updateProjectionMatrix();
    }

private:
    /**
     * \brief Calculates the front vector from the Camera's (updated) Euler Angles
     */
    void updateCameraVectors()
    {
        // calculate the new Front vector
        glm::dvec3 newFront;
        newFront.x = cos(glm::radians(yawE)) * cos(glm::radians(pitchE));
        newFront.y = sin(glm::radians(pitchE));
        newFront.z = sin(glm::radians(yawE)) * cos(glm::radians(pitchE));
        front = glm::normalize(newFront);
        // also re-calculate the Right and Up vector
        right = glm::normalize(glm::cross(front, worldUp));  // normalize the vectors, because their length gets closer to 0 the more you look up or down which results in slower movement.
        up = glm::normalize(glm::cross(right, front));
    }

    void updatePosition(glm::vec3 posOffset)
    {
        float posOffset_m = fmodf((posOffset.x + position_m.x), 100000.0f);
        float posOffset_M = ((posOffset.x + position_m.x) - posOffset_m) / coef_M;
        position_m.x = posOffset_m;
        position_M.x += posOffset_M;

        posOffset_m = fmodf((posOffset.y + position_m.y), 100000.0f);
        posOffset_M = ((posOffset.y + position_m.y) - posOffset_m) / coef_M;
        position_m.y = posOffset_m;
        position_M.y += posOffset_M;

        posOffset_m = fmodf((posOffset.z + position_m.z), 100000.0f);
        posOffset_M = ((posOffset.z + position_m.z) - posOffset_m) / coef_M;
        position_m.z = posOffset_m;
        position_M.z += posOffset_M;
    }
};