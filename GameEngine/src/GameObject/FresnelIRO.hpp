#pragma once
#include <unordered_map>
#include <glm/glm.hpp>

enum FresnelIRO {
	DEFAULT, // ±âº»°ª, 0.04f
	WATER,
	BIO_TISSUE,
	SKIN,
	EYE,
	HAIR,
	TOOTH,
	FABRIC,
	STONE,
	PLASTIC,
	CRYSTAL,
	DIAMOND,

	// METAL
	TITANIUM,
	CHROME,
	IRON,
	NICKEL,
	PLATINUM,
	GOLD,
	COPPER,
	SILVER,
	ALUMINUM,
};

extern std::unordered_map<FresnelIRO, glm::vec3> FresnelData;