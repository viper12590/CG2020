#include "LightSource.h"

LightSource::LightSource(glm::vec3 position, float intensity) {
    LightSource::pos = position;
    LightSource::intensity = intensity;
}