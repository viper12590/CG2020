#include <CanvasTriangle.h>
#include <DrawingWindow.h>
#include <Utils.h>
#include <fstream>
#include <vector>
#include <glm/glm.hpp>
#include <CanvasPoint.h>
#include <Colour.h>
#include <CanvasTriangle.h>
#include <TextureMap.h>
#include <time.h>
#include "ModelTriangle.h"
#include "math.h"
#include "Material.h"
#include "RayTriangleIntersection.h"
#include <glm/gtx/string_cast.hpp>
#include "ObjLoader.h"

#define WIDTH 600
#define HEIGHT 400

std::vector<std::vector<float>> ZBuffer;
std::vector<std::pair<ModelTriangle, Material>> pairs;
bool orbitMode = false;

enum RenderMode { WIREFRAME, RASTERIZING, RAYTRACING };

RenderMode renderMode = WIREFRAME;

class Camera {
	public:
		glm::vec3 pos = glm::vec3(0.0, 0.0, 4.0);
		glm::mat3 rot = glm::mat3(1.0);
		float f = 2.0;
		float speed = 0.05;
		double rSpeed = glm::radians(0.5);
};

Camera camera;
glm::vec3 CENTER(0.0,0.0,0.0);
glm::vec3 lightSource;


std::vector<float> interpolateSingleFloats(float from, float to, int numberOfValues) {
	std::vector<float> values(numberOfValues);

	float interval = (to - from) / (numberOfValues - 1);
	for(int i = 0; i < numberOfValues; i++) values[i] = from + i*interval;

	return values;
}

std::vector<glm::vec3> interpolateVector(glm::vec3 from, glm::vec3 to, int numberOfValues) {
	std::vector<glm::vec3> values(numberOfValues);

	glm::vec3 intervals = (to - from) / (float)(numberOfValues - 1);
	for(int i = 0; i < numberOfValues; i++) values[i] = from + (float)i*intervals;

	return values;
}

std::vector<glm::vec2> interpolateVector(glm::vec2 from, glm::vec2 to, int numberOfValues) {
	std::vector<glm::vec2> values(numberOfValues);

	glm::vec2 intervals = (to - from) / (float)(numberOfValues - 1);
	for(int i = 0; i < numberOfValues; i++) values[i] = from + (float)i*intervals;

	return values;
}

std::vector<CanvasPoint> interpolateVector(CanvasPoint from, CanvasPoint to, int numberOfValues) {
	std::vector<CanvasPoint> values(numberOfValues);

	glm::vec3 intervals = (glm::vec3(to.x,to.y,to.depth) - glm::vec3(from.x,from.y,from.depth)) / (float)(numberOfValues);
	for(int i = 0; i < numberOfValues; i++) {
		glm::vec3 result = glm::vec3(from.x, from.y, from.depth) + (float)i*intervals;
		//for debugging
		if(glm::isnan(result.x)) {
			std::cout << "nan intervals: " << intervals.x << " num: " << numberOfValues << " to.x: "<< to.x << " from.x: " << from.x << std::endl;
			std::cout << "from.y: " << from.y << " to.y: " << to.y << std::endl;
		}
		values[i] = CanvasPoint(result.x,result.y, result.z);
	} 		


	return values;
}

void drawLine(DrawingWindow &window, CanvasPoint from, CanvasPoint to, Colour colour) {
	glm::vec3 src(from.x, from.y, from.depth);
	glm::vec3 dest(to.x, to.y, to.depth);
	glm::vec3 difference = dest - src;
	float numberOfSteps = glm::max(glm::abs(difference.x), glm::abs(difference.y));
	glm::vec3 step = difference/numberOfSteps;	
	for(float i = 0; i < numberOfSteps; i++) {
		glm::vec3 pixel = src + step * i;
		//add pixel to depth buffer if bigger than whats there already
		int X = (int)glm::floor(pixel.x);
		int Y = (int)glm::floor(pixel.y);
		if(X < 0 || X > WIDTH - 1 || Y < 0 || Y > HEIGHT - 1) {
			continue;
		}
		if(pixel.z == 0) {
			ZBuffer[X][Y] = pixel.z;
			window.setPixelColour(X, Y, colour.toHex(0xFF));
		}
		else if(pixel.z > (ZBuffer[X][Y])) {
			ZBuffer[X][Y] = pixel.z;
			window.setPixelColour(X, Y, colour.toHex(0xFF));
		}
	}
}

void drawTriangle(DrawingWindow &window, CanvasTriangle triangle, Colour colour) {
	drawLine(window, triangle.v0(), triangle.v1(), colour);
	drawLine(window, triangle.v1(), triangle.v2(), colour);
	drawLine(window, triangle.v2(), triangle.v0(), colour);
}

std::vector<CanvasPoint> getSortedTriangeVertices(std::vector<CanvasPoint> vertices) {
	if(vertices[0].y > vertices[1].y) std::swap(vertices[0], vertices[1]);
	if(vertices[1].y > vertices[2].y) std::swap(vertices[1], vertices[2]);
	if(vertices[0].y > vertices[1].y) std::swap(vertices[0], vertices[1]);
	return vertices;
}

CanvasPoint getTriangleExtraPoint(CanvasPoint top, CanvasPoint middle, CanvasPoint bottom) {
	float deltaX = bottom.x - top.x;
	float deltaY = bottom.y - top.y;
	
	float gradient = deltaX / deltaY;
	float Y =  middle.y;
	float relY = (middle.y - top.y);
	float X = top.x + relY * gradient;
	
	float depth = INFINITY;
	float distanceTB = glm::length(glm::vec2(deltaX,deltaY));
	float distanceTE = glm::length(glm::vec2((X - top.x),(Y - top.y)));
	float zDeltaTB = bottom.depth - top.depth;
	float zDeltaTE = (zDeltaTB*distanceTE)/distanceTB ;
	depth = zDeltaTE + top.depth;
	
	return CanvasPoint(X,Y,depth);
}


std::vector<CanvasPoint> rasterizeTriangle(CanvasTriangle triangle) {
	std::vector<CanvasPoint> vertices;
	vertices.resize(3);
	for(int i = 0; i < 3; i++) vertices[i] = triangle.vertices[i];

	//Sorting CanvasPoints
	std::vector<CanvasPoint> sorted = getSortedTriangeVertices(vertices);

	CanvasPoint top = sorted[0];
	CanvasPoint middle = sorted[1];
	CanvasPoint bottom = sorted[2];
	//Finding extra point
	CanvasPoint extra = getTriangleExtraPoint(top,middle,bottom);
	std::vector<CanvasPoint> result;
	result.resize(4);
	result[0] = top;
	result[1] = middle;
	result[2] = bottom;
	result[3] = extra;

	return result;
}

std::vector<CanvasPoint> rasterizeTriangle(std::vector<CanvasPoint> vertices) {
	//Assume the vertices are already sorted
	//Finding extra point
	CanvasPoint extra = getTriangleExtraPoint(vertices[0],vertices[1],vertices[2]); //top mid bottom
	std::vector<CanvasPoint> result;
	result.resize(4);
	result[0] = vertices[0];//top
	result[1] = vertices[1]; //mid
	result[2] = vertices[2]; //bottom
	result[3] = extra;

	return result;
}

void fillTriangle(DrawingWindow &window, CanvasTriangle triangle, Colour colour) {
	std::vector<CanvasPoint> vertices = rasterizeTriangle(triangle);
	CanvasPoint top = vertices[0];
	CanvasPoint middle = vertices[1];
	CanvasPoint bottom = vertices[2];
	CanvasPoint extra = vertices[3];
	
	//side to side interpolation
	std::vector<CanvasPoint> topToMiddle = interpolateVector(top, middle, (int)(middle.y - top.y)+1);
	std::vector<CanvasPoint> topToExtra = interpolateVector(top, extra, (int)(extra.y - top.y)+1);
	std::vector<CanvasPoint> bottomToMiddle = interpolateVector(bottom, middle, (int)(bottom.y - middle.y)+1);
	std::vector<CanvasPoint> bottomToExtra = interpolateVector(bottom, extra, (int)(bottom.y - middle.y)+1);

	//filling top part
	for(int i=0; i < topToMiddle.size(); i++) {
		drawLine(window, topToMiddle[i], topToExtra[i], colour);
	}
	//filling bottom part
	for(int i=0; i < bottomToMiddle.size(); i++) {
		drawLine(window, bottomToMiddle[i], bottomToExtra[i], colour);
	}
}

void drawFilledTriangle(DrawingWindow &window, CanvasTriangle triangle, Colour lineColour, Colour fillColour) {
	fillTriangle(window, triangle, fillColour);
	drawTriangle(window,triangle,lineColour);
}

CanvasPoint calcExtraTexturePoint(std::vector<CanvasPoint> rCanvas, std::vector<CanvasPoint> rTexture) {
	glm::vec3 cTopToExtra = glm::vec3(rCanvas[3].x, rCanvas[3].y, rCanvas[3].depth) - glm::vec3(rCanvas[0].x, rCanvas[0].y, rCanvas[0].depth);
	glm::vec3 cTopToBottom = glm::vec3(rCanvas[2].x, rCanvas[2].y, rCanvas[2].depth) - glm::vec3(rCanvas[0].x, rCanvas[0].y, rCanvas[0].depth);
	glm::vec3 ratio = cTopToExtra / cTopToBottom;
	glm::vec3 tTopToBottom = glm::vec3(rTexture[2].x, rTexture[2].y, rTexture[2].depth) - glm::vec3(rTexture[0].x, rTexture[0].y,  rTexture[0].depth);
	glm::vec3 tTopToExtra = tTopToBottom * ratio;

	//add depth

	return CanvasPoint(rTexture[0].x + tTopToExtra.x, rTexture[0].y + tTopToExtra.y, rTexture[0].depth + tTopToExtra.z);
}


void drawTexturedTriangle(DrawingWindow &window, CanvasTriangle triangle, std::string path) {
	TextureMap texture(path);
	std::vector<CanvasPoint> initCanvasPoints({triangle.v0(),triangle.v1(),triangle.v2()});
	std::vector<CanvasPoint> sortedCanvasPoints = getSortedTriangeVertices(initCanvasPoints);
	
	CanvasPoint tTop(sortedCanvasPoints[0].texturePoint.x, sortedCanvasPoints[0].texturePoint.y);
	CanvasPoint tMiddle(sortedCanvasPoints[1].texturePoint.x, sortedCanvasPoints[1].texturePoint.y);
	CanvasPoint tBottom(sortedCanvasPoints[2].texturePoint.x, sortedCanvasPoints[2].texturePoint.y);

	std::vector<CanvasPoint> rCanvas = rasterizeTriangle(sortedCanvasPoints);
	std::vector<CanvasPoint> rTexture({tTop, tMiddle, tBottom});

	//Getting extra for texture
	CanvasPoint tExtra = calcExtraTexturePoint(rCanvas, rTexture);
	if(glm::isnan(rCanvas[1].x)) std::cout << "bad" << std::endl;
	//Interpolate canvas
	std::vector<CanvasPoint> cTopToMiddleI = interpolateVector(rCanvas[0], rCanvas[1], (int)(rCanvas[1].y- rCanvas[0].y)+1);
	std::vector<CanvasPoint> cTopToExtraI = interpolateVector(rCanvas[0], rCanvas[3], (int)(rCanvas[3].y - rCanvas[0].y)+1);
	std::vector<CanvasPoint> cBottomToMiddleI = interpolateVector(rCanvas[2], rCanvas[1], (int)(rCanvas[2].y - rCanvas[1].y)+1);
	std::vector<CanvasPoint> cBottomToExtraI = interpolateVector(rCanvas[2], rCanvas[3], (int)(rCanvas[2].y - rCanvas[3].y)+1);

	//Interpolate texure
	std::vector<CanvasPoint> tTopToMiddleI = interpolateVector(tTop, tMiddle, (int)(rCanvas[1].y- rCanvas[0].y)+1);
	std::vector<CanvasPoint> tTopToExtraI = interpolateVector(tTop, tExtra, (int)(rCanvas[3].y - rCanvas[0].y)+1);
	std::vector<CanvasPoint> tBottomToMiddleI = interpolateVector(tBottom, tMiddle, (int)(rCanvas[2].y- rCanvas[1].y)+1);
	std::vector<CanvasPoint> tBottomToExtraI = interpolateVector(tBottom, tExtra, (int)(rCanvas[2].y - rCanvas[3].y)+1);

	//Sample and paint top triangle
	for(int i = 0; i < tTopToMiddleI.size(); i++) {
		glm::vec3 tSrc(tTopToMiddleI[i].x, tTopToMiddleI[i].y, tTopToMiddleI[i].depth);
		glm::vec3 tDest(tTopToExtraI[i].x, tTopToExtraI[i].y, tTopToExtraI[i].depth);

		glm::vec3 cSrc(cTopToMiddleI[i].x, cTopToMiddleI[i].y, cTopToMiddleI[i].depth);
		glm::vec3 cDest(cTopToExtraI[i].x, cTopToExtraI[i].y, cTopToExtraI[i].depth);
		//if(glm::isnan(cSrc.x)) std::cout << i << std::endl;
		glm::vec3 tDifference = tDest - tSrc;
		glm::vec3 cDifference = cDest - cSrc;
		float numberOfSteps = glm::max(glm::abs(cDifference.x), glm::abs(cDifference.y));
		glm::vec3 tStep = tDifference/numberOfSteps;
		glm::vec3 cStep = cDifference/numberOfSteps;	
		for(float j = 0; j < numberOfSteps; j++) {
			glm::vec3 tPixel = glm::floor(tSrc + tStep * j);
			glm::vec3 cPixel = cSrc + cStep * j;
			uint32_t colour = texture.pixels[(int)tPixel.x + (texture.width) * (int)tPixel.y];
			//add pixel to depth buffer if bigger than whats there already
			int X = (int) glm::floor(cPixel.x);
			int Y = (int) glm::floor(cPixel.y);
			if(X < 0 || X > WIDTH - 1 || Y < 0 || Y > HEIGHT - 1) {
				continue;
			}
			if(cPixel.z == 0) {
				ZBuffer[X][Y] = cPixel.z;
				window.setPixelColour(X, Y, colour);
			}
			else if(cPixel.z > (ZBuffer[X][Y])) {
				ZBuffer[X][Y] = cPixel.z;
				window.setPixelColour(X, Y, colour);
			}
			
		}

	}

	//Sample and paint bottom triangle
	for(int i = 0; i < tBottomToMiddleI.size(); i++) {
		glm::vec3 tSrc(tBottomToMiddleI[i].x, tBottomToMiddleI[i].y, tBottomToMiddleI[i].depth);
		glm::vec3 tDest(tBottomToExtraI[i].x, tBottomToExtraI[i].y, tBottomToExtraI[i].depth);
		
		glm::vec3 cSrc(cBottomToMiddleI[i].x, cBottomToMiddleI[i].y, cBottomToMiddleI[i].depth);
		glm::vec3 cDest(cBottomToExtraI[i].x, cBottomToExtraI[i].y, cBottomToExtraI[i].depth);
		glm::vec3 tDifference = tDest - tSrc;
		glm::vec3 cDifference = cDest - cSrc;
		float numberOfSteps = glm::max(glm::abs(cDifference.x), glm::abs(cDifference.y));
		glm::vec3 tStep = tDifference/numberOfSteps;
		glm::vec3 cStep = cDifference/numberOfSteps;	
		for(float j = 0; j < numberOfSteps; j++) {
			glm::vec3 tPixel = glm::floor(tSrc + tStep * j);
			glm::vec3 cPixel = cSrc + cStep * j;
			uint32_t colour = texture.pixels[(int)tPixel.x + (texture.width) * (int)tPixel.y];
			//add pixel to depth buffer if bigger than whats there already
			int X = (int) glm::floor(cPixel.x);
			int Y = (int) glm::floor(cPixel.y);
			if(X < 0 || X > WIDTH - 1 || Y < 0 || Y > HEIGHT - 1) {
				continue;
			}
			if(cPixel.z == 0) {
				ZBuffer[X][Y] = cPixel.z;
				window.setPixelColour(X, Y, colour);
			}
			else if(cPixel.z > (ZBuffer[X][Y])) {
				ZBuffer[X][Y] = cPixel.z;
				window.setPixelColour(X, Y, colour);
			}
		}
	}	
	
}

//Calculate possible solution of ray tracing intersection
glm::vec3 getPossibleIntersectionSolution(ModelTriangle triangle, glm::vec3 rayOrigin ,glm::vec3 rayDirection) {
	glm::vec3 e0 = triangle.vertices[1] - triangle.vertices[0];
	glm::vec3 e1 = triangle.vertices[2] - triangle.vertices[0];
	glm::vec3 SPVector = rayOrigin - triangle.vertices[0];
	glm::mat3 DEMatrix(-rayDirection, e0, e1);
	glm::vec3 possibleSolution = glm::inverse(DEMatrix) * SPVector;
	return possibleSolution;
}

//Check if the possible solution is valid (inside the triangle)
bool isValidIntersection(glm::vec3 tuvVector) {
	float t = tuvVector[0];
	float u = tuvVector[1];
	float v = tuvVector[2];
	return (u >= 0.0) && (u <= 1.0) && (v >= 0.0) && (v <= 1.0) && (u + v) <= 1.0 && t > 0.0;
}

//Calculate 3D coordinate of the intersection
RayTriangleIntersection getRayTriangleIntersection(ModelTriangle triangle, glm::vec3 validVector) {
	glm::vec3 e0 = triangle.vertices[1] - triangle.vertices[0];
	glm::vec3 e1 = triangle.vertices[2] - triangle.vertices[0];
	float t = validVector[0];
	float u = validVector[1];
	float v = validVector[2];
	glm::vec3 r = triangle.vertices[0] + u*e0 + v*e1;
	RayTriangleIntersection result = RayTriangleIntersection();
	result.intersectionPoint = r;
	result.distanceFromCamera = t;
	result.intersectedTriangle = triangle;
	return result;
}


CanvasTriangle getRandomTriangle() {
	CanvasPoint point0((float)(rand() % WIDTH), (float)(rand() % HEIGHT));
	CanvasPoint point1((float)(rand() % WIDTH), (float)(rand() % HEIGHT));
	CanvasPoint point2((float)(rand() % WIDTH), (float)(rand() % HEIGHT));
	CanvasTriangle triangle(point0, point1, point2);
	
	return triangle;
}


CanvasTriangle getCanvasTriangle(ModelTriangle triangle) {
	std::vector<glm::vec3> renderPos;
	for(int i=0; i < triangle.vertices.size(); i++) {
		glm::vec3 vertex = triangle.vertices[i] - camera.pos;
		vertex = camera.rot * vertex;
		float u = glm::floor(-camera.f*(vertex.x / vertex.z)*WIDTH + WIDTH/2);
		float v = glm::floor(camera.f*(vertex.y / vertex.z)*WIDTH + HEIGHT/2);
		float Z = INFINITY;
		
		if(vertex.z != 0.0) {
			Z = glm::abs(1 / vertex.z);
		}
		renderPos.push_back(glm::vec3(u, v, Z));
	}
	return CanvasTriangle(CanvasPoint(renderPos[0].x, renderPos[0].y, renderPos[0].z),CanvasPoint(renderPos[1].x, renderPos[1].y, renderPos[1].z) ,CanvasPoint(renderPos[2].x, renderPos[2].y, renderPos[2].z));			
}

void drawModelTriangle(DrawingWindow &window, std::pair<ModelTriangle, Material> pair) {
	ModelTriangle triangle = pair.first;
	Material material = pair.second;
	CanvasTriangle transposedTri = getCanvasTriangle(triangle);

	if(material.texturePath.empty()) {
		drawFilledTriangle(window, transposedTri, triangle.colour, triangle.colour);
		// if(colourPack(triangle.colour,0xFF) == 0xFFFFFFFF) {
		// 	std::cout << glm::to_string(triangle.vertices[0]) << " " << glm::to_string(triangle.vertices[1]) << " " << glm::to_string(triangle.vertices[2]) << std::endl;
		// }
	} else {		
		//drawFilledTriangle(window, transposedTri, triangle.colour, triangle.colour);
		TextureMap textureMap(material.texturePath);

		transposedTri.v0().texturePoint = TexturePoint(triangle.texturePoints[0].x * textureMap.width, textureMap.height - triangle.texturePoints[0].y * textureMap.height);
		transposedTri.v1().texturePoint = TexturePoint(triangle.texturePoints[1].x * textureMap.width, textureMap.height - triangle.texturePoints[1].y * textureMap.height);
		transposedTri.v2().texturePoint = TexturePoint(triangle.texturePoints[2].x * textureMap.width, textureMap.height - triangle.texturePoints[2].y * textureMap.height);
		drawTexturedTriangle(window, transposedTri, material.texturePath);
	}
}

glm::mat3 lookAt(glm::vec3 source, glm::vec3 target) {
	glm::vec3 forward = glm::normalize(source - target);
	glm::vec3 right = -glm::normalize(glm::cross(forward, glm::vec3(0,1,0)));
	glm::vec3 up = -glm::normalize(glm::cross(forward, -right));
	return glm::transpose(glm::mat3(right, up, forward));
}

bool triangleEqual(ModelTriangle triangle1, ModelTriangle triangle2) {
	return triangle1.vertices[0] == triangle2.vertices[0] && triangle1.vertices[1] == triangle2.vertices[1] && triangle1.vertices[2] == triangle2.vertices[2];
}

bool shadowRay(int u, int v, std::vector<std::pair<ModelTriangle,Material>> pairs, RayTriangleIntersection intersection, glm::vec3 light) {
	glm::vec3 worldSpaceCanvasPixel = intersection.intersectionPoint;
	glm::vec3 rayDirection = glm::normalize((light - worldSpaceCanvasPixel));
	bool isShadow = false;
	for(int i=0; i < pairs.size(); i++) {
		ModelTriangle triangle = pairs[i].first;
		if(!triangleEqual(triangle,intersection.intersectedTriangle)) {

			glm::vec3 tuvVector = getPossibleIntersectionSolution(triangle, worldSpaceCanvasPixel, rayDirection);
			if(isValidIntersection(tuvVector) && tuvVector[0] < glm::length(light - worldSpaceCanvasPixel)) {
				isShadow = true;
			}
		}
	}
	return isShadow;
}

void rayTracing(DrawingWindow &window, std::vector<std::pair<ModelTriangle,Material>> pairs, float scale) {
	for(int u = 0; u < WIDTH; u++) {
		for(int v = 0; v < HEIGHT; v++) {
			glm::vec3 cameraSpaceCanvasPixel((u - WIDTH/2), (HEIGHT/2 - v), -camera.f*WIDTH);
			glm::vec3 worldSpaceCanvasPixel = (cameraSpaceCanvasPixel * camera.rot) + camera.pos;
			glm::vec3 rayDirection = glm::normalize(worldSpaceCanvasPixel - camera.pos);
			std::vector<RayTriangleIntersection> intersections;
			std::vector<Material> materials;
			
			for(int i = 0; i < pairs.size(); i++) {
				ModelTriangle triangle = pairs[i].first;
				Material material = pairs[i].second;
				glm::vec3 tuvVector = getPossibleIntersectionSolution(triangle, camera.pos, rayDirection);
				if(isValidIntersection(tuvVector)) {
					RayTriangleIntersection intersection = getRayTriangleIntersection(triangle, tuvVector);
					intersections.push_back(intersection);
					materials.push_back(material);
				}
			}
			if(!intersections.empty()) {
				
				RayTriangleIntersection closest = intersections[0];
				Material closestMat = materials[0];
				for(int i = 0; i < intersections.size(); i++) {
					if(intersections[i].distanceFromCamera <= closest.distanceFromCamera){
						closest = intersections[i];
						closestMat = materials[i];
					}
				}
				
				if(shadowRay(u,v,pairs,closest,lightSource)) {
					window.setPixelColour(u,v,0xFF000000);
				}
				else {
					window.setPixelColour(u,v,closestMat.colour.toHex(0xFF));
				}
			}
		}
	
	}
}


void handleEvent(SDL_Event event, DrawingWindow &window) {
	if (event.type == SDL_KEYDOWN) {
		if (event.key.keysym.sym == SDLK_LEFT) {
			camera.pos = glm::vec3(camera.pos.x - camera.speed, camera.pos.y, camera.pos.z);
		}
		else if (event.key.keysym.sym == SDLK_RIGHT) {
			camera.pos = glm::vec3(camera.pos.x + camera.speed, camera.pos.y, camera.pos.z);
		}
		else if (event.key.keysym.sym == SDLK_UP) {
			camera.pos = glm::vec3(camera.pos.x, camera.pos.y, camera.pos.z - camera.speed);
		}
		else if (event.key.keysym.sym == SDLK_DOWN) {
			camera.pos = glm::vec3(camera.pos.x, camera.pos.y, camera.pos.z + camera.speed);
		}
		else if (event.key.keysym.sym == SDLK_x) {
			camera.pos = glm::vec3(camera.pos.x, camera.pos.y  + camera.speed, camera.pos.z);
		}
		else if (event.key.keysym.sym == SDLK_z) {
			camera.pos = glm::vec3(camera.pos.x, camera.pos.y  - camera.speed, camera.pos.z);
		}
		else if (event.key.keysym.sym == SDLK_w) {
			glm::mat3 rotationMat(
									1.0,0.0,0.0,
									0.0,glm::cos(-camera.rSpeed),glm::sin(-camera.rSpeed),
									0.0, -glm::sin(-camera.rSpeed),glm::cos(-camera.rSpeed) 
			);
			camera.rot = rotationMat * camera.rot;
		}
		else if (event.key.keysym.sym == SDLK_s) {
			glm::mat3 rotationMat(
									1.0,0.0,0.0,
									0.0,glm::cos(camera.rSpeed),glm::sin(camera.rSpeed),
									0.0, -glm::sin(camera.rSpeed),glm::cos(camera.rSpeed) 
			);
			camera.rot = rotationMat * camera.rot;
		}
		else if (event.key.keysym.sym == SDLK_a) {
			glm::mat3 rotationMat(
									glm::cos(-camera.rSpeed),0.0,-glm::sin(-camera.rSpeed),
									0.0,1.0,0.0,
									glm::sin(-camera.rSpeed),0.0,glm::cos(-camera.rSpeed) 
			);
			camera.rot = rotationMat * camera.rot;
		}
		else if (event.key.keysym.sym == SDLK_d) {

			glm::mat3 rotationMat(
									glm::cos(camera.rSpeed),0.0,-glm::sin(camera.rSpeed),
									0.0,1.0,0.0,
									glm::sin(camera.rSpeed),0.0,glm::cos(camera.rSpeed) 
			);
			camera.rot = rotationMat * camera.rot;
		}
		else if (event.key.keysym.sym == SDLK_q) {
			glm::mat3 rotationMat(
									glm::cos(-camera.rSpeed),glm::sin(-camera.rSpeed),0.0,
									-glm::sin(-camera.rSpeed),glm::cos(-camera.rSpeed),0.0,
									0.0,0.0,1.0
			);
			camera.rot = rotationMat * camera.rot;
		}
		else if (event.key.keysym.sym == SDLK_e) {
			glm::mat3 rotationMat(
									glm::cos(camera.rSpeed),glm::sin(camera.rSpeed),0.0,
									-glm::sin(camera.rSpeed),glm::cos(camera.rSpeed),0.0,
									0.0,0.0,1.0
			);
			camera.rot = rotationMat * camera.rot;
		}
		else if (event.key.keysym.sym == SDLK_SPACE) {
			std::cout << "look at the center" << std::endl;
			camera.rot = lookAt(camera.pos, CENTER);
		}
		else if(event.key.keysym.sym == SDLK_m) {
			std::cout << "Rasterizing" << std::endl;
			renderMode = RASTERIZING;
		}
		else if(event.key.keysym.sym == SDLK_o) {
			orbitMode = !orbitMode;
		}
		else if(event.key.keysym.sym == SDLK_r) {
			std::cout << "RayTracing" << std::endl;
			renderMode = RAYTRACING;
		}
		else if(event.key.keysym.sym == SDLK_n) {
			std::cout << "Wireframe" << std::endl;
			renderMode = WIREFRAME;
		}
	} else if (event.type == SDL_MOUSEBUTTONDOWN) window.savePPM("output.ppm");
}

void draw(DrawingWindow &window) {
	for(int x = 0; x < WIDTH; x++) {
		for(int y = 0; y < HEIGHT; y++) {
			ZBuffer[x][y] = 0.0;
		}
	}
	window.clearPixels();
	switch (renderMode) {
		case WIREFRAME:
			for(int i=0; i < pairs.size(); i++) {
				ModelTriangle triangle = pairs[i].first;
				Material material = pairs[i].second;
				CanvasTriangle transposedTri = getCanvasTriangle(triangle);
				drawTriangle(window,transposedTri,material.colour);
			}
			break;
		case RASTERIZING:
			for(int i=0; i < pairs.size(); i++) {
				drawModelTriangle(window, pairs[i]);
			}
			break;
		case RAYTRACING:
			rayTracing(window, pairs, 750.0);
			break;
		default:
			break;
	}
	//Render light source position
	glm::vec3 vertex = lightSource - camera.pos;
	vertex = camera.rot * vertex;
	float u = glm::floor(-1*camera.f*(vertex.x / vertex.z)*(HEIGHT*1.5)+ WIDTH/2);
	float v = glm::floor(camera.f*(vertex.y / vertex.z)*(HEIGHT*1.5) + HEIGHT/2);
	window.setPixelColour(u,v,0xFFFF0000);
	//window.setPixelColour(432,39,0x00FF0000);
	
}
float theta = glm::acos(camera.pos.x / glm::distance(glm::vec3(0.0), camera.pos));

void orbit() {
	float radius = glm::distance(glm::vec3(0.0), camera.pos);
	float interval = glm::radians(5.0);
	theta += interval;
	float newX = radius*glm::cos(theta);
	float newZ = radius*glm::sin(theta);
	camera.pos = glm::vec3(newX, camera.pos.y, newZ);
	camera.rot = lookAt(camera.pos, CENTER);
}
// Function for performing animation (shifting artifacts or moving the camera)
void update(DrawingWindow &window) {
	if(orbitMode) orbit();

}


int main(int argc, char *argv[]) {
	srand(time(NULL));
	ZBuffer.resize(WIDTH);
	for(int x = 0; x < WIDTH; x++) {
		ZBuffer[x].resize(HEIGHT);
		for(int y = 0; y < HEIGHT; y++) {
			ZBuffer[x][y] = 0.0;
		}
	}
	ObjLoader modelLoader = ObjLoader();
	pairs = modelLoader.loadObj("textured-cornell-box.obj", 0.17);
	lightSource = glm::vec3(0.0, 0.45, 0.0);

	DrawingWindow window = DrawingWindow(WIDTH, HEIGHT, false);
	SDL_Event event;
	while (true) {
		// We MUST poll for events - otherwise the window will freeze !
		if (window.pollForInputEvents(event)) handleEvent(event, window);
		update(window);
		draw(window);
		// Need to render the frame at the end, or nothing actually gets shown on the screen !
		window.renderFrame();
	}
}
