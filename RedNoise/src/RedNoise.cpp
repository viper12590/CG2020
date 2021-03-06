#include <CanvasTriangle.h>
#include <DrawingWindow.h>
#include <Utils.h>
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
#include "Camera.h"
#include "LightSource.h"
#include <sstream>
#include <iomanip>

#define WIDTH 640
#define HEIGHT 480
#define AMBIENCE 0.2f
#define SHADOW_BIAS 0.001f

std::vector<std::vector<float>> ZBuffer;
std::vector<std::pair<ModelTriangle, Material>> pairs;
std::vector<std::pair<ModelTriangle, Material>> cornell_box;
std::vector<std::pair<ModelTriangle, Material>> sphere;
int model = 0;
enum RenderMode { WIREFRAME, RASTERIZING, RAYTRACING };
RenderMode renderMode = WIREFRAME;
bool orbitMode = false;
bool simple_animation = false;
bool record = false;
bool phong = false;
Camera camera;
glm::vec3 CENTER(0.0,0.0,0.0);
LightSource lightSource(glm::vec3(0.0, 0.36, 0.1),2.0);
std::vector<LightSource> lightSources;
uint32_t MIRROR_COLOUR = 0xFFFF00FF;
uint32_t GLASS_COLOUR = 0xFFFF0000;
float vaccumRI = 1.0f;
float glassRI = 1.2f;


std::vector<CanvasPoint> interpolateVector(CanvasPoint from, CanvasPoint to, int numberOfValues) {
	std::vector<CanvasPoint> values(numberOfValues);

	glm::vec3 intervals = (glm::vec3(to.x,to.y,to.depth) - glm::vec3(from.x,from.y,from.depth)) / (float)(numberOfValues);
	for(int i = 0; i < numberOfValues; i++) {
		glm::vec3 result = glm::vec3(from.x, from.y, from.depth) + (float)i*intervals;
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
	float ratioX = cTopToExtra.x/cTopToBottom.x;
	float ratioY = cTopToExtra.y/cTopToBottom.y;
	float ratioZ = cTopToExtra.z/cTopToBottom.z;
	if(glm::isnan(ratioX)) {
		ratioX = 0.0f;
	}
	if(glm::isnan(ratioY)) {
		ratioY = 0.0f;
	}
	if(glm::isnan(ratioZ)) {
		ratioZ = 0.0f;
	}
	glm::vec3 ratio = glm::vec3(ratioX,ratioY,ratioZ);
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

CanvasPoint getCanvasPoint(glm::vec3 vertex) {
	vertex -= camera.pos;
	vertex = camera.rot * vertex;
	float u = glm::floor(-camera.f*(vertex.x / vertex.z)*WIDTH + WIDTH/2);
	float v = glm::floor(camera.f*(vertex.y / vertex.z)*WIDTH + HEIGHT/2);
	float Z = INFINITY;
	
	if(vertex.z != 0.0) {
		Z = glm::abs(1 / vertex.z);
	}
	return CanvasPoint(u,v,Z);
}


CanvasTriangle getCanvasTriangle(ModelTriangle triangle) {
	std::vector<CanvasPoint> points;
	for(int i=0; i < triangle.vertices.size(); i++) {
		CanvasPoint point = getCanvasPoint(triangle.vertices[i]);
		points.push_back(point);
	}
	return CanvasTriangle(points[0],points[1],points[2]);			
}

void drawModelTriangle(DrawingWindow &window, std::pair<ModelTriangle, Material> pair) {
	ModelTriangle triangle = pair.first;
	Material material = pair.second;
	CanvasTriangle transposedTri = getCanvasTriangle(triangle);

	if(material.texturePath.empty()) {
		drawFilledTriangle(window, transposedTri, triangle.colour, triangle.colour);
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

void wireframeRender(DrawingWindow &window, std::vector<std::pair<ModelTriangle, Material>> modelPairs) {
	for(int i=0; i < modelPairs.size(); i++) {
		ModelTriangle triangle = modelPairs[i].first;
		Material material = modelPairs[i].second;
		CanvasTriangle transposedTri = getCanvasTriangle(triangle);
		drawTriangle(window,transposedTri,material.colour);
	}
	CanvasPoint light = getCanvasPoint(lightSource.pos);
	if(light.x >= 0 && light.x <= WIDTH - 1 && light.y >= 0 && light.y <= HEIGHT - 1) {
		window.setPixelColour(light.x,light.y,0xFF00FF00);
	}
}

void rasterisingRender(DrawingWindow &window, std::vector<std::pair<ModelTriangle, Material>> modelPairs) {
	for(int i=0; i < modelPairs.size(); i++) {
		drawModelTriangle(window, modelPairs[i]);
	}
}

bool shadowRay(ModelTriangle triangle, RayTriangleIntersection intersection, glm::vec3 light) {
	glm::vec3 worldSpaceCanvasPixel = intersection.intersectionPoint;
	glm::vec3 rayDirection = glm::normalize((light - worldSpaceCanvasPixel));
	if(!triangleEqual(triangle,intersection.intersectedTriangle)) {
		glm::vec3 tuvVector = getPossibleIntersectionSolution(triangle, worldSpaceCanvasPixel, rayDirection);
		if(isValidIntersection(tuvVector) && tuvVector[0] < glm::length(light - worldSpaceCanvasPixel)) {
			return true;
		}
	}
	return false;
}

//With vertex normal
bool shadowRay(ModelTriangle triangle, RayTriangleIntersection intersection, glm::vec3 normal, glm::vec3 light, float shadowBias) {
	glm::vec3 worldSpaceCanvasPixel = intersection.intersectionPoint + shadowBias*normal;
	glm::vec3 rayDirection = glm::normalize((light - worldSpaceCanvasPixel));
	if(!triangleEqual(triangle,intersection.intersectedTriangle)) {
		glm::vec3 tuvVector = getPossibleIntersectionSolution(triangle, worldSpaceCanvasPixel, rayDirection);
		if(isValidIntersection(tuvVector) && tuvVector[0] < glm::length(light - worldSpaceCanvasPixel)) {
			return true;
		}
	}
	return false;
}

std::array<glm::vec3, 3> calcTriangleVertexNormal(ModelTriangle triangle, std::vector<std::pair<ModelTriangle,Material>> pairs) {
	std::array<glm::vec3, 3> vertexNormals;
	//For all vertex of the current triangle
	for(int v = 0; v < 3; v++) {
		glm::vec3 currentV = triangle.vertices[v];
		std::vector<ModelTriangle> neighbourTriangles;
		//Check every ModelTriangles and add every neighbours to vector (including self)
		for(int i = 0; i < pairs.size(); i++) {
			ModelTriangle modelTriagnle = pairs[i].first;
			if(currentV == modelTriagnle.vertices[0] || currentV == modelTriagnle.vertices[1] || currentV == modelTriagnle.vertices[2]) {
				neighbourTriangles.push_back(modelTriagnle);
			}
		}

		glm::vec3 totalNormal(0);
		for(int j = 0; j < neighbourTriangles.size(); j++) {
			//Add all triangle normal of neighbours (and self)
			totalNormal += neighbourTriangles[j].normal;
		}
		//Take the average
		totalNormal /= neighbourTriangles.size();
		vertexNormals[v] = glm::normalize(totalNormal);
	}
	return vertexNormals;
}

//Interpolating with barycentric coordinates
float getGouraud(float v0, float v1, float v2, float u, float v) {
	float w = 1.0f - (u + v);
	float gouraud = (v*v2 + u*v1 + w*v0);
	return gouraud;
}

glm::vec3 getVertexNormal(ModelTriangle triangle, float u, float v) {
	float w = 1.0 - (u + v);
	glm::vec3 targetNormal = glm::normalize(v*triangle.vertexNormals[2] + u*triangle.vertexNormals[1] + w*triangle.vertexNormals[0]);
	return targetNormal;
}

bool isInShadow(RayTriangleIntersection intersection, LightSource lightSource) {
	bool shadow = false;
	for(int i = 0; i < pairs.size(); i++) {
		ModelTriangle triangle = pairs[i].first;
		if(shadowRay(triangle,intersection,lightSource.pos)) {
			shadow = true;
			break;	
		}
	}
	return shadow;
}

//With vertex normal
bool isInShadow(RayTriangleIntersection intersection, glm::vec3 vertexNormal, LightSource lightSource, float shadowBias) {
	bool shadow = false;
	for(int i = 0; i < pairs.size(); i++) {
		ModelTriangle triangle = pairs[i].first;
		if(shadowRay(triangle,intersection,vertexNormal,lightSource.pos,shadowBias)) {
			shadow = true;
			break;	
		}
	}
	return shadow;
}

float getProximityBrightness(glm::vec3 target, LightSource light) {
	float distance = glm::abs(glm::length(light.pos - target));
	float brightness = 1.0f/(glm::pi<float>()*distance*distance);
	return glm::clamp(brightness, 0.0f, 1.0f);
}

glm::vec3 getNormalOfTriangle(ModelTriangle triangle) {
	glm::vec3 edge0 = triangle.vertices[1] - triangle.vertices[0];
	glm::vec3 edge1 = triangle.vertices[2] - triangle.vertices[0];
	glm::vec3 normal = glm::normalize(glm::cross(edge0, edge1));
	return normal;
}

float getAngleOfIncidence(glm::vec3 target, glm::vec3 normal ,LightSource light) {
	glm::vec3 lightDirection = glm::normalize(light.pos - target);
	float angle = glm::dot(normal,lightDirection);
	return glm::clamp(angle, 0.0f, 1.0f);
}

glm::vec3 getVectorOfReflection(glm::vec3 normal, glm::vec3 from) {
	glm::vec3 fromDirection = glm::normalize(from);
	return glm::normalize(fromDirection - 2.0f*normal*(glm::dot(fromDirection,normal)));
}

glm::vec3 getVectorOfRefraction(glm::vec3 from, glm::vec3 normal, float n1, float n2) {
	from = glm::normalize(from);
	normal = glm::normalize(normal);
	float r = n1/n2;
	float c = glm::dot(normal, from);
	if(c < 0.0f) {
		c = glm::dot(-normal, from);
	}
	float sine1 = glm::clamp<float>(1.0f - glm::pow(c,2), 0.0f, 1.0f);
	float sine2 = glm::clamp<float>(1.0f - glm::pow(r,2) * sine1, 0.0f, 1.0f);
	glm::vec3 refraction = r*from + (float)(r*c - sine2) * normal;

	return refraction;
}

float getSpecularSpread(glm::vec3 target,glm::vec3 normal ,Camera view, LightSource light, int shininess) {
	glm::vec3 viewDirection = glm::normalize(view.pos - target);
	glm::vec3 reflection = getVectorOfReflection(normal,light.pos);
	float spread = glm::pow(glm::dot(viewDirection,reflection),shininess);
	return spread;
}

//For gouraud
float getTotalBrightness(glm::vec3 targetVertex, glm::vec3 vertexNormal, float ambience) {
	float brightness = getProximityBrightness(targetVertex,lightSource);
	float angleOfIncidence = getAngleOfIncidence(targetVertex,vertexNormal,lightSource);
	float lighting = glm::max(ambience, brightness * angleOfIncidence);
	return lighting;
}

Colour getLightAffectedColour(glm::vec3 targetVertex, Colour targetColour, LightSource lightSource, float ambience, bool shadow, glm::vec3 vertexNormal) {
	if(!shadow) {
		float brightness = getProximityBrightness(targetVertex,lightSource);
		float angleOfIncidence = getAngleOfIncidence(targetVertex,vertexNormal,lightSource);
		float lighting = glm::max(ambience, brightness * angleOfIncidence);

		float specular = 255.0f * getSpecularSpread(targetVertex,vertexNormal,camera,lightSource,256);
		glm::vec3 finalColourVector = glm::clamp((specular + lightSource.intensity * lighting * glm::vec3(targetColour.red, targetColour.green, targetColour.blue)),0.0f,255.0f);
		Colour finalColour(finalColourVector.r,finalColourVector.g,finalColourVector.b);
		return finalColour;
	}
	glm::vec3 finalColourVector = glm::vec3(targetColour.red, targetColour.green, targetColour.blue) * ambience;
	Colour finalColour(finalColourVector.r, finalColourVector.g, finalColourVector.b);
	return finalColour;
}

void rayTrace2(int x, int y, DrawingWindow &window, RayTriangleIntersection from, glm::vec3 rayDirection, std::vector<std::pair<ModelTriangle,Material>>& pairs, int recursive) {
	if(recursive > 0) {
		
		std::vector<RayTriangleIntersection> intersections;
		std::vector<glm::vec3> tuvVectors;

		//Getting ray intersections
		for(std::pair<ModelTriangle,Material>& pair : pairs) {
			ModelTriangle triangle = pair.first;
			if((from.intersectedTriangle.isMirror && triangle.isMirror) || (from.intersectedTriangle.isGlass && triangle.isGlass)) {
				continue;
			}
			glm::vec3 tuvVector = getPossibleIntersectionSolution(triangle, from.intersectionPoint, rayDirection);
			if(isValidIntersection(tuvVector)) {
				tuvVectors.push_back(tuvVector);
				RayTriangleIntersection intersection = getRayTriangleIntersection(triangle, tuvVector);
				intersections.push_back(intersection);
			}
		}

		//When ray hit ModelTriangle
		if(!intersections.empty()) {
			//Find closest intersection point
			RayTriangleIntersection closest = intersections[0];
			glm::vec3 closestTuvVector = tuvVectors[0];
			for(int i = 0; i < intersections.size(); i++) {
				if(intersections[i].distanceFromCamera <= closest.distanceFromCamera){
					closest = intersections[i];
					closestTuvVector = tuvVectors[i];
				}
			}
			//Get normal of the vertex
			glm::vec3 vertexNormal = getVertexNormal(closest.intersectedTriangle,closestTuvVector[1],closestTuvVector[2]);

			//Check for reflection
			if(closest.intersectedTriangle.isMirror) {
				//Mirror reflection
				glm::vec3 rayDirection = closest.intersectionPoint - camera.pos;
				glm::vec3 reflectedRay = getVectorOfReflection(closest.intersectedTriangle.normal,rayDirection);
				rayTrace2(x,y,window,closest,reflectedRay,pairs,recursive-1);
			}
			//Check for refraction
			else if(closest.intersectedTriangle.isGlass) {
				//refraction
				glm::vec3 rayDirection = closest.intersectionPoint - camera.pos;
				glm::vec3 refractedRay;
				if(from.intersectedTriangle.isGlass) {
					refractedRay = getVectorOfRefraction(rayDirection,closest.intersectedTriangle.normal,closest.intersectedTriangle.refractiveIndex,vaccumRI);
				}
				else {
					refractedRay = getVectorOfRefraction(rayDirection,closest.intersectedTriangle.normal,vaccumRI,closest.intersectedTriangle.refractiveIndex);
				}
				rayTrace2(x,y,window,closest,refractedRay,pairs,recursive-1);
			}
			else {
				if(from.intersectedTriangle.isMirror) {
					bool reflectedShadow = isInShadow(closest,closest.intersectedTriangle.normal,lightSource,SHADOW_BIAS);
					Colour reflectedColour = getLightAffectedColour(closest.intersectionPoint, closest.intersectedTriangle.colour, lightSource, AMBIENCE, reflectedShadow, closest.intersectedTriangle.normal);
					Colour finalColour = getLightAffectedColour(from.intersectionPoint,reflectedColour,lightSource,0.35f,false,from.intersectedTriangle.normal);
					window.setPixelColour(x,y,finalColour.toHex(0xFF));
				}
				else if(from.intersectedTriangle.isGlass) {
					bool refractedShadow = isInShadow(closest,closest.intersectedTriangle.normal,lightSource,SHADOW_BIAS);
					Colour refractedColour = getLightAffectedColour(closest.intersectionPoint, closest.intersectedTriangle.colour, lightSource, AMBIENCE, refractedShadow, closest.intersectedTriangle.normal);
					window.setPixelColour(x,y,refractedColour.toHex(0xFF));	
				}
				else {
					//Check for the shadow
					bool shadow = isInShadow(closest,vertexNormal,lightSource,SHADOW_BIAS);
					Colour finalColour = getLightAffectedColour(closest.intersectionPoint,closest.intersectedTriangle.colour,lightSource,AMBIENCE,shadow,vertexNormal);
					window.setPixelColour(x,y,finalColour.toHex(0xFF));	
				}
			}
		}
	}
}


void rayTrace(int x, int y, DrawingWindow &window, std::vector<std::pair<ModelTriangle,Material>>& pairs, int recursive) {
	if(recursive > 0) {
		glm::vec3 cameraSpaceCanvasPixel((x - WIDTH/2), (HEIGHT/2 - y), -camera.f*WIDTH);
		glm::vec3 worldSpaceCanvasPixel = (cameraSpaceCanvasPixel * camera.rot) + camera.pos;
		glm::vec3 rayDirection = glm::normalize(worldSpaceCanvasPixel - camera.pos);
		std::vector<RayTriangleIntersection> intersections;
		std::vector<glm::vec3> tuvVectors;

		//Getting ray intersections
		for(std::pair<ModelTriangle,Material>& pair : pairs) {
			ModelTriangle triangle = pair.first;
			glm::vec3 tuvVector = getPossibleIntersectionSolution(triangle, camera.pos, rayDirection);
			if(isValidIntersection(tuvVector)) {
				tuvVectors.push_back(tuvVector);
				RayTriangleIntersection intersection = getRayTriangleIntersection(triangle, tuvVector);
				intersections.push_back(intersection);
			}
		}

		//When ray hit ModelTriangle
		if(!intersections.empty()) {
			//Find closest intersection point
			RayTriangleIntersection closest = intersections[0];
			glm::vec3 closestTuvVector = tuvVectors[0];
			for(int i = 0; i < intersections.size(); i++) {
				if(intersections[i].distanceFromCamera <= closest.distanceFromCamera){
					closest = intersections[i];
					closestTuvVector = tuvVectors[i];
				}
			}
			//Get normal of the vertex
			glm::vec3 vertexNormal = getVertexNormal(closest.intersectedTriangle,closestTuvVector[1],closestTuvVector[2]);
			if(phong) {
				
				//Check reflection
				if(closest.intersectedTriangle.isMirror) {
					//Mirror reflection
					glm::vec3 rayDirection = closest.intersectionPoint - camera.pos;
					glm::vec3 reflectedRay = getVectorOfReflection(closest.intersectedTriangle.normal,rayDirection);
					rayTrace2(x,y,window,closest,reflectedRay,pairs,recursive-1);
				}
				else if(closest.intersectedTriangle.isGlass) {
					//Refraction
					glm::vec3 rayDirection = closest.intersectionPoint - camera.pos;
					glm::vec3 refractedRay = getVectorOfRefraction(rayDirection,closest.intersectedTriangle.normal,vaccumRI,closest.intersectedTriangle.refractiveIndex);
					rayTrace2(x,y,window,closest,refractedRay,pairs,recursive-1);
				}
				else {
					//Check for the shadow
					bool shadow = isInShadow(closest,vertexNormal,lightSource,SHADOW_BIAS);
					Colour finalColour = getLightAffectedColour(closest.intersectionPoint,closest.intersectedTriangle.colour,lightSource,AMBIENCE,shadow,vertexNormal);
					window.setPixelColour(x,y,finalColour.toHex(0xFF));
				}
			}
			else {
				//else gouraud shading
				ModelTriangle tri = closest.intersectedTriangle;
				Colour targetColour = tri.colour;

				float v0Brightness = getTotalBrightness(tri.vertices[0],tri.vertexNormals[0],AMBIENCE);
				float v1Brightness = getTotalBrightness(tri.vertices[1],tri.vertexNormals[1],AMBIENCE);
				float v2Brightness = getTotalBrightness(tri.vertices[2],tri.vertexNormals[2],AMBIENCE);
				float gouraudBrightness = glm::clamp(getGouraud(v0Brightness,v1Brightness,v2Brightness,closestTuvVector[1],closestTuvVector[2]), AMBIENCE, 1.0f);
				
				float v0Specular = getSpecularSpread(tri.vertices[0],tri.vertexNormals[0],camera,lightSource,256);
				float v1Specular = getSpecularSpread(tri.vertices[1],tri.vertexNormals[1],camera,lightSource,256);
				float v2Specular = getSpecularSpread(tri.vertices[2],tri.vertexNormals[2],camera,lightSource,256);
				float gouraudSpecular = 255.0f * getGouraud(v0Specular,v1Specular,v2Specular,closestTuvVector[1],closestTuvVector[2]);

				bool shadow = isInShadow(closest,lightSource);
				if(!shadow) {
					glm::vec3 finalColourVector = glm::clamp((gouraudSpecular + lightSource.intensity * gouraudBrightness * glm::vec3(targetColour.red, targetColour.green, targetColour.blue)),0.0f,255.0f);
					Colour finalColour(finalColourVector.r,finalColourVector.g,finalColourVector.b);
					window.setPixelColour(x,y,finalColour.toHex(0xFF));
				}
				else {
					glm::vec3 finalColourVector = glm::vec3(targetColour.red, targetColour.green, targetColour.blue) * AMBIENCE;
					Colour finalColour(finalColourVector.r, finalColourVector.g, finalColourVector.b);
					window.setPixelColour(x,y,finalColour.toHex(0xFF));
				}
			}
		}
	}
}

void raytracingRender(DrawingWindow &window, std::vector<std::pair<ModelTriangle,Material>>& pairs) {
	for(int x = 0; x < WIDTH; x++) {
		for(int y = 0; y < HEIGHT; y++) {
			rayTrace(x,y,window,pairs,10);
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
		else if(event.key.keysym.sym == SDLK_o) {
			orbitMode = !orbitMode;
		}
		else if(event.key.keysym.sym == SDLK_p) {
			simple_animation = !simple_animation;
			std::cout << "simple animation:" << simple_animation << std::endl;
		}
		else if(event.key.keysym.sym == SDLK_m) {
			std::cout << "Rasterizing" << std::endl;
			renderMode = RASTERIZING;
		}
		else if(event.key.keysym.sym == SDLK_r) {
			std::cout << "RayTracing" << std::endl;
			renderMode = RAYTRACING;
		}
		else if(event.key.keysym.sym == SDLK_n) {
			std::cout << "Wireframe" << std::endl;
			renderMode = WIREFRAME;
		}
		else if(event.key.keysym.sym == SDLK_h) {
			lightSource.pos.y -= 0.01;
		}
		else if(event.key.keysym.sym == SDLK_y) {
			lightSource.pos.y += 0.01;
		}
		else if(event.key.keysym.sym == SDLK_g) {
			lightSource.pos.x -= 0.01;
		}
		else if(event.key.keysym.sym == SDLK_j) {
			lightSource.pos.x += 0.01;
		}
		else if(event.key.keysym.sym == SDLK_u) {
			lightSource.pos.z -= 0.01;
		}
		else if(event.key.keysym.sym == SDLK_t) {
			lightSource.pos.z += 0.01;
		}
		else if(event.key.keysym.sym == SDLK_LCTRL) {
			phong = !phong;
			std::cout << "phong:" << phong << std::endl;
		}
		else if(event.key.keysym.sym == SDLK_v) {
			record = !record;
			std::cout << "record:" << record << std::endl;
		}
		else if(event.key.keysym.sym == SDLK_RETURN) {
			switch (model) {
			case 0:
				lightSource.pos = glm::vec3(-0.1, 0.46, 0.5);
				lightSource.intensity = 1.0;
				pairs = sphere;
				camera.f = 4.0;
				camera.pos = glm::vec3(0.0,0.2,4.0);
				model = 1;
				break;
			case 1:
			default:
				lightSource.pos = glm::vec3(0.0, 0.36, 0.1);
				lightSource.intensity = 2.0;
				pairs = cornell_box;
				camera.f = 2.0;
				camera.pos = glm::vec3(0.0,0.0,4.0);
				model = 0;
				break;
			}
		}
	} 
	else if (event.type == SDL_MOUSEBUTTONDOWN) {
		window.savePPM("output.ppm");
	};
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
			wireframeRender(window, pairs);
			break;
		case RASTERIZING:
			rasterisingRender(window,pairs);
			break;
		case RAYTRACING:
			raytracingRender(window, pairs);
			break;
		default:
			break;
	}
	
}
float theta = glm::acos(camera.pos.x / glm::distance(glm::vec3(0.0), camera.pos));

void orbit() {
	float radius = glm::distance(glm::vec3(0.0), camera.pos);
	float interval = glm::radians(5.0);
	theta -= interval;
	float newX = radius*glm::cos(theta);
	float newZ = radius*glm::sin(theta);
	camera.pos = glm::vec3(newX, camera.pos.y, newZ);
	camera.rot = lookAt(camera.pos, CENTER);
}

void simpleAnimation() {
	for(int i = 0; i < pairs.size(); i++) {
		if(pairs[i].first.colour.toHex(0xFF) == 0xFF0000FF || pairs[i].first.colour.toHex(0xFF) == 0xFFFF0000) {
			if(pairs[i].first.vertices[0].y <= 0.38f && pairs[i].first.vertices[1].y <= 0.38f && pairs[i].first.vertices[2].y <= 0.38f) {
				for(int v = 0; v < 3; v++) {
					pairs[i].first.vertices[v].y += 0.01f;
				}
			}
		}
	}
}

void saveRender(DrawingWindow &window, int frameNumber) {
	std::stringstream ss;
	ss << std::setw(10) << std::setfill('0') << frameNumber << ".ppm";
	window.savePPM(ss.str());
}

// Function for performing animation (shifting artifacts or moving the camera)
void update(DrawingWindow &window) {
	if(orbitMode) orbit();
	if(simple_animation) simpleAnimation();
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
	sphere = modelLoader.loadObj("sphere.obj", 0.17);
	for(int i = 0; i < sphere.size(); i++) {
		sphere[i].first.normal = getNormalOfTriangle(sphere[i].first);
	}

	cornell_box = modelLoader.loadObj("textured-cornell-box.obj",0.17);
	for(int i = 0; i < cornell_box.size(); i++) {
		cornell_box[i].first.normal = getNormalOfTriangle(cornell_box[i].first);
	}
	for(int i = 0; i < cornell_box.size(); i++) {
		cornell_box[i].first.vertexNormals = calcTriangleVertexNormal(cornell_box[i].first, cornell_box);
		if(cornell_box[i].first.colour.toHex(0xFF) == MIRROR_COLOUR) {
			cornell_box[i].first.isMirror = true;
		}
		if(cornell_box[i].first.colour.toHex(0xFF) == GLASS_COLOUR) {
			cornell_box[i].first.isGlass = true;
			cornell_box[i].first.refractiveIndex = glassRI;
		}
	}

	pairs = cornell_box;

	DrawingWindow window = DrawingWindow(WIDTH, HEIGHT, false);
	SDL_Event event;
	int frame = 0;
	while (true) {
		// We MUST poll for events - otherwise the window will freeze !
		if (window.pollForInputEvents(event)) handleEvent(event, window);
		update(window);
		draw(window);
		// Need to render the frame at the end, or nothing actually gets shown on the screen !
		window.renderFrame();
		if(record) {
			saveRender(window, frame);
			frame++;
		}
	}
}
