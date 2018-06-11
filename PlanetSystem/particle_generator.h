#pragma once
/*******************************************************************
** This code is part of Breakout.
**
** Breakout is free software: you can redistribute it and/or modify
** it under the terms of the CC BY 4.0 license as published by
** Creative Commons, either version 4 of the License, or (at your
** option) any later version.
******************************************************************/
#ifndef PARTICLE_GENERATOR_H
#define PARTICLE_GENERATOR_H
#include <vector>
#include <iostream>

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "shader.h"
#include "texture.h"

#define RED_COLOR glm::vec4(1.0f, 1.0f, 0.0f, 1.0f)

// Represents a single particle and its state
struct Particle {
	glm::vec3 Position, Velocity, Acceleration;
	glm::vec4 Color;
	GLfloat Life;
	GLboolean Visible;

	Particle() : Position(0.0f), Velocity(0.0f), Color(1.0f), Life(0.0f), Visible(GL_FALSE) { }
};


// ParticleGenerator acts as a container for rendering a large number of 
// particles by repeatedly spawning and updating particles and killing 
// them after a given amount of time.
class ParticleGenerator
{
public:
	// Constructor
	ParticleGenerator(Shader shader, Texture2D texture, GLuint amount);
	// Update all particles
	void Update(GLfloat dt, GLuint newParticles, glm::vec3 centerPos = glm::vec3(0.0f, 0.0f, 0.0f));
	// Render all particles
	void Draw();
private:
	// State
	std::vector<Particle> particles;
	GLuint amount;
	// Render state
	Shader shader;
	Texture2D texture;
	GLuint VAO;
	// element buffer size, use in glDrawElements
	GLuint indexCount;
	// Initializes buffer and vertex attributes
	void init();
	// Returns the first Particle index that's currently unused e.g. Life <= 0.0f or 0 if no particle is currently inactive
	GLuint firstUnusedParticle();
	// Respawns particle
	void respawnParticle(Particle &particle, glm::vec3 centerPos = glm::vec3(0.0f, 0.0f, 0.0f));
};

#endif