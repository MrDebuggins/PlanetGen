#pragma once

//#include <glad/glad.h>
#include <chrono>
#include <ctime>
#include <iostream>
#include <GL/freeglut_std.h>
#include <GL/glew.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>


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
    DOWN = 'f'
};

// Default camera values
constexpr float yaw_c = -90.0f;
constexpr float pitch_c = 0.0f;
constexpr float speed_c = 10000.0f;
constexpr float sensitivity_c = 0.1f;
constexpr float zoom_c = 45.0f;

/**
 * \brief An abstract camera class that processes input and calculates the corresponding Euler Angles,
 * Vectors and Matrices for use in OpenGL
 */
class Camera
{
public:
    // camera timer
    time_t timer;

    // camera Attributes
    glm::vec3 position;
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

    std::chrono::high_resolution_clock::time_point lastUpdateTime;

    int w = 700;
    int h = 400;
    
    /**
     * \brief Constructor with vectors and default options.
     * \param position camera position
     * \param up normalized camera orientation
     * \param yaw camera yaw
     * \param pitch camera pitch
     */
    Camera(glm::vec3 position = glm::vec3(0.0f, 1.0f * 6371001.0f, 0.0f), glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f), float yaw = yaw_c, float pitch = pitch_c) : front(glm::vec3(0.0f, 0.0f, -1.0f)), movementSpeed(speed_c), mouseSensitivity(sensitivity_c), zoom(zoom_c)
    {
        this->position = position;
        worldUp = up;
        this->yawE = yaw;
        this->pitchE = pitch;
        lastUpdateTime = std::chrono::high_resolution_clock::now();
        updateCameraVectors();
    }

    /**
     * \brief Returns the view matrix calculated using Euler Angles and the LookAt Matrix
     * \return view matrix
     */
    glm::mat4 getViewMatrix() const
    {
        glm::vec3 tmp = position + front;
        return glm::lookAt(glm::vec3(0, 0, 0), front, up);
        //return glm::lookAt(position, tmp, up);
    }
    
    /**
     * \brief Processes input received from any keyboard input system.
     * Accepts input parameter in the form of camera defined ENUM (to abstract it from windowing systems)
     * \param direction key pressed (w,a,s,d keys correspond to camera_movement enum members)
     * \param delta_time time camera has been moving
     * \param state true if button pressed, false otherwise
     */
    void processKeyboard(const camera_movement direction, const float delta_time, bool state)
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
    }

    void update()
    {
        std::chrono::high_resolution_clock::time_point tmp = std::chrono::high_resolution_clock::now();
        //std::cout << tmp - lastUpdateTime << std::endl;
        const float velocity = movementSpeed * std::chrono::duration_cast<std::chrono::duration<double>>(tmp - lastUpdateTime).count();
        lastUpdateTime = tmp;

        if(buttons[0])
            position += front * velocity;
        if (buttons[1])
            position -= front * velocity;
        if (buttons[2])
            position -= right * velocity;
        if (buttons[3])
            position += right * velocity;
        if (buttons[4])
            position += up * velocity;
        if (buttons[5])
            position -= up * velocity;
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

        yawE += x * mouseSensitivity;
        pitchE += -y * mouseSensitivity;

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
        zoom -= static_cast<float>(y_offset);
        if (zoom < 1.0f)
            zoom = 1.0f;
        if (zoom > 45.0f)
            zoom = 45.0f;
    }

private:
    /**
     * \brief Calculates the front vector from the Camera's (updated) Euler Angles
     */
    void updateCameraVectors()
    {
        // calculate the new Front vector
        glm::vec3 newFront;
        newFront.x = cos(glm::radians(yawE)) * cos(glm::radians(pitchE));
        newFront.y = sin(glm::radians(pitchE));
        newFront.z = sin(glm::radians(yawE)) * cos(glm::radians(pitchE));
        front = glm::normalize(newFront);
        // also re-calculate the Right and Up vector
        right = glm::normalize(glm::cross(front, worldUp));  // normalize the vectors, because their length gets closer to 0 the more you look up or down which results in slower movement.
        up = glm::normalize(glm::cross(right, front));
    }
};