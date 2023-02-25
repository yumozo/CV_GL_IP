#ifndef CAMERA_H
#define CAMERA_H

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <vector>

enum CameraMovement { FORWARD, BACKWARD, LEFT, RIGHT };

/* Def camera values */
const float YAW = -90.f;
const float PITCH = 0.f;
const float SPEED = 2.5f;
const float SENSITIVITY = .1f;
const float ZOOM = 45.f;

class Camera {
   public:
    /* Camera attribs*/
    glm::vec3 m_Position;
    glm::vec3 m_Front;
    glm::vec3 m_Up;
    glm::vec3 m_Right;
    glm::vec3 m_WorldUp;
    /* Euler angles */
    float m_Yaw;
    float m_Pitch;
    /* Camera options */
    float m_MovementSpeed;
    float m_MouseSensitivity;
    float m_Zoom;
    Camera( glm::vec3 position = glm::vec3( 0.f, 0.f, 0.f ),
            glm::vec3 up = glm::vec3( 0.f, 1.f, 0.f ), float yaw = YAW,
            float pitch = PITCH );
    Camera( float posX, float posY, float posZ, float upX, float upY, float upZ,
            float yaw, float pitch );

    glm::mat4 GetViewMatrix();

    void ProcessKeyboard( CameraMovement direction, float deltaTime ) {
        /* Move it */
        // float velocity = m_MovementSpeed * deltaTime;
        // if(direction == FORWARD){
        //     m_Position += m_Front * velocity;
        // }
        // if(direction == BACKWARD){
        //     m_Position -= m_Front * velocity;
        // }
        // if(direction == RIGHT){
        //     m_Position += m_Right * velocity;
        // }
        // if(direction == LEFT){
        //     m_Position -= m_Right * velocity;
        // }
    }
    void ProcessMouseScroll( float yOffset );
    private:
     void UpdateCameraVectors();
};
#endif