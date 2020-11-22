#pragma once

#include <glm/glm.hpp>

struct LightSource {
    public:
        LightSource(glm::vec3 position, float intensity);
        glm::vec3 pos;
        float intensity;
};