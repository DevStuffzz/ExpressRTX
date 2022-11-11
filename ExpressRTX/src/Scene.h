#include <glm/glm.hpp>

#include <vector>

struct Light {
	glm::vec3 Position{ 0.0f };
	float Radius = 1.0f;
	glm::vec3 Color = glm::vec3(1.0f);
};

struct Material {

	glm::vec3 Albedo{ 1.0f };

	float Roughness = 1.0f;
	float Metallic = 0.0f;
	float Specular = 0.0f;
};

struct Sphere
{
	glm::vec3 Position{ 0.0f };
	float Radius = 0.5f;

	int MaterialIndex = 0;
};

struct Scene
{
	std::vector<Sphere> Spheres;
	std::vector<Material> Materials;
	std::vector<Light> Lights;
	float Attenuation = 0.333f;

};