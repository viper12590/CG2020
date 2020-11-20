#include "Material.h"

Material::Material() {
    colour = Colour();
}

Material::Material(Colour colour,  std::string texturePath) {
    Material::colour = colour;
    Material::texturePath = texturePath;
}