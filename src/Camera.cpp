#include "Camera.h"
#include <gtc/matrix_transform.hpp>



Core::Camera::Camera (glm::vec3 pos,glm::vec3 fr){
    position = pos;
    front = fr;
    forward = pos + fr;
    createViewMatrix();
    perspective = glm::perspectiveFov(45.f, 1280.0f, 720.0f, 0.01f, 100.0f);
}


void Core::Camera::createViewMatrix()
{
    glm::vec3 cameraTarget = forward;
    glm::vec3 fwd = glm::normalize(position - cameraTarget);
    glm::vec3 sup = up;
    glm::vec3 side = glm::normalize(glm::cross(sup, fwd));
    sup = glm::cross(fwd, side);
    // Pamietaj o tym, ze macierz perspektywy patrzy w kierunku -z.
    glm::mat4 cameraRotation = { side.x,sup.x,fwd.x,0.0f,
                                side.y,sup.y,fwd.y,0.0f,
                                side.z,sup.z,fwd.z,0.0f,
                                0.0f,0.0f,0.0f,1.0f,

    };
    glm::mat4 cameraTranslation = { 1.0f,0.0f,0.0f,0.0f,
                                    0.0f,1.0f,0.0f,0.0f,
                                    0.0f,0.0f,1.0f,0.0f,
                                    -position.x,-position.y,-position.z,1.0f,

    };
    view = cameraRotation * cameraTranslation;
}

void Core::Camera::update() {
    forward = position + front;
    createViewMatrix();
}

glm::vec3 Core::Camera::getPosition()
{
    return position;
}

void Core::Camera::setPosition(glm::vec3 pos)
{
    position = pos;
}

glm::vec3 Core::Camera::getForward()
{
    return forward;
}

void Core::Camera::setForward(glm::vec3 fwd)
{
    forward = fwd;
}

glm::vec3 Core::Camera::getFront()
{
    return front;
}

void Core::Camera::setFront(glm::vec3 fr)
{
    front = fr;
}

glm::mat4 Core::Camera::getPerspective()
{
    return perspective;
}

void Core::Camera::setPerspective(glm::mat4 per)
{
    perspective = per;
}

glm::mat4 Core::Camera::getView()
{
    return view;
}

void Core::Camera::setView(glm::mat4 vw)
{
    view = vw;
}




