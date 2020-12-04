#include "ObjLoader.h"

ObjLoader::ObjLoader() {
}

Material ObjLoader::loadMaterial(std::string mtlName, std::string mtlPath) {
    std::ifstream in(mtlPath, std::ifstream::in);
	std::string line;
	glm::vec3 colour;
	std::string texturePath;
	
    bool found = false;
    while(!found && !in.eof()) {
	    std::getline(in, line);
        std::vector<std::string> tokens = split(line, ' ');
        if(tokens[0].compare("newmtl") == 0 && tokens[1].compare(mtlName) == 0) {
            found = true;
        }
    }
    found = false;
	while(!in.eof() && !found) {
        std::getline(in, line);
        std::vector<std::string> tokens = split(line, ' ');
        if(tokens[0].compare("Kd") == 0) {
            colour = glm::vec3(0xFF*stof(tokens[1]), 0xFF*stof(tokens[2]), 0xFF*stof(tokens[3]));
        }
		if(tokens[0].compare("map_Kd") == 0) {
			texturePath = tokens[1];
		}
		if(tokens[0].compare("newmtl") == 0) {
			found = true;
		}
    }
	Material material = Material(Colour(colour.r, colour.g, colour.b), texturePath);
    in.close();
    return material;
}


std::vector<std::pair<ModelTriangle,Material>> ObjLoader::loadObj(std::string filePath, float scale) {
    std::ifstream file(filePath, std::ifstream::in);
	std::string line;
	
	std::vector<std::pair<ModelTriangle, Material>> pairs;
	std::vector<glm::vec3> vertices;
	std::vector<glm::vec3> vertexNormals;
	std::vector<TexturePoint> texturePoints;

	Material material;
	std::string mtlPath;


	while(!file.eof()) {
		std::getline(file, line);
		std::vector<std::string> tokens = split(line, ' ');

		if(tokens[0].compare("mtllib") == 0) mtlPath = tokens[1];
		else if(tokens[0].compare("usemtl") == 0) material = loadMaterial(tokens[1], mtlPath);
		else if(tokens[0].compare("v") == 0) vertices.push_back(scale * glm::vec3(stof(tokens[1]), stof(tokens[2]), stof(tokens[3])));
		else if(tokens[0].compare("vt") == 0) {
			texturePoints.push_back(TexturePoint(stof(tokens[1]), stof(tokens[2])));
		}
		else if(tokens[0].compare("vn") == 0) vertexNormals.push_back(glm::vec3(stof(tokens[1]),stof(tokens[2]),stof(tokens[3])));
		
		else if(tokens[0].compare("f") == 0) {
			//For each index in f
			std::array<glm::vec3, 3> trianglePoints;
			std::array<TexturePoint, 3> selectedTexturePoints;
			std::array<glm::vec3, 3> selectedNormals;
			for(int i = 1; i < 4; i++) {
				std::vector<std::string> subTokens = split(tokens[i], '/');
				//TrianglePoint
				trianglePoints[i - 1] = vertices[stoi(subTokens[0]) - 1];
				//TexturePoint
				if(!texturePoints.empty() && subTokens[1].compare("\0") != 0) {
					selectedTexturePoints[i - 1] = texturePoints[stoi(subTokens[1]) - 1];
				}
				if(!vertexNormals.empty() && subTokens[2].compare("\0") != 0) {
					selectedNormals[i - 1] = vertexNormals[stoi(subTokens[2]) - 1];
				}
			}

			ModelTriangle triangle = ModelTriangle();
			triangle.vertices = trianglePoints;
			triangle.texturePoints = selectedTexturePoints;
			triangle.colour = material.colour;
			triangle.vertexNormals = selectedNormals;
			pairs.emplace_back(triangle,material);
		}
	}

	file.close();	
	return pairs;
}


