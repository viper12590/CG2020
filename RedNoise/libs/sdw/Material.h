#pragma once

#include "Colour.h"
#include "TextureMap.h"

class Material {
public:
    Colour colour;
    std::string texturePath;
    Material();
    Material(Colour colour,  std::string texturePath);
};