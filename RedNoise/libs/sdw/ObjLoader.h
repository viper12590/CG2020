#pragma once

#include "ModelTriangle.h"
#include "Material.h"

class ObjLoader {
public:
    ObjLoader();
    std::vector<std::pair<ModelTriangle,Material>> loadObj(std::string filePath, float scale);

private:
    Material loadMaterial(std::string name, std::string path);
};
