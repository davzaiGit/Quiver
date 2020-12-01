#include "Camera.h"

glm::mat4 Core::createViewMatrix( glm::vec3 position, glm::vec3 forward, glm::vec3 up )
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
    return cameraRotation * cameraTranslation;
}
