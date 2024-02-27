#include "FresnelIRO.hpp"

std::unordered_map<FresnelIRO, glm::vec3> FresnelData = {
	{FresnelIRO::DEFAULT, glm::vec3(0.04f)},
	{FresnelIRO::WATER, glm::vec3(0.02f)},
	{FresnelIRO::BIO_TISSUE, glm::vec3(0.04f)},
	{FresnelIRO::SKIN, glm::vec3(0.028f)},
	{FresnelIRO::EYE, glm::vec3(0.025f)},
	{FresnelIRO::HAIR, glm::vec3(0.046f)},
	{FresnelIRO::TOOTH, glm::vec3(0.058f)},
	{FresnelIRO::FABRIC, glm::vec3(0.04f)},
	{FresnelIRO::STONE, glm::vec3(0.035f)},
	{FresnelIRO::PLASTIC, glm::vec3(0.04f)},
	{FresnelIRO::CRYSTAL, glm::vec3(0.05f)},
	{FresnelIRO::DIAMOND, glm::vec3(0.13f)},

	// METAL
	{FresnelIRO::TITANIUM, glm::vec3(0.542,0.497,0.449)},
	{FresnelIRO::CHROME, glm::vec3(0.549,0.556,0.554)},
	{FresnelIRO::IRON, glm::vec3(0.562,0.565,0.554)},
	{FresnelIRO::NICKEL, glm::vec3(0.660,0.609,0.526)},
	{FresnelIRO::PLATINUM, glm::vec3(0.673,0.637,0.565)},
	{FresnelIRO::GOLD, glm::vec3(1.000,0.782,0.344)},
	{FresnelIRO::COPPER, glm::vec3(0.955,0.638,0.538)},
	{FresnelIRO::SILVER, glm::vec3(0.972,0.960,0.915)},
	{FresnelIRO::ALUMINUM, glm::vec3(0.913,0.922,0.924)},
};

