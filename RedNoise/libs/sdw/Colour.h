#pragma once

#include <iostream>

struct Colour {
	std::string name;
	int red{};
	int green{};
	int blue{};
	Colour();
	Colour(int r, int g, int b);
	Colour(std::string n, int r, int g, int b);
	uint32_t toHex(int alpha);
};

std::ostream &operator<<(std::ostream &os, const Colour &colour);
