#pragma once
#ifndef PLANET_SYSTEM_H
#define PLANET_SYSTEM_H
#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <shader.h>
#include <vector>
#include <map>
#include <string>

struct Planet {
	glm::vec3 Position, Velocity;
	GLfloat Scale;
	GLfloat Mass;
	glm::vec4 Color;
	Planet() :Position(0.0f), Scale(1.0f), Mass(0.0f), Color(1.0f) {}
};

class PlanetSystem
{
public:
	PlanetSystem(Shader shader);
	void Update(GLfloat dt);
	void Draw();
private:
	std::vector<Planet> planets;
	GLuint amout;
	Shader shader;
	GLuint sphereVAO;
	GLuint indexCount;
	void init();
};
#endif