/*******************************************************************
** This code is part of Breakout.
**
** Breakout is free software: you can redistribute it and/or modify
** it under the terms of the CC BY 4.0 license as published by
** Creative Commons, either version 4 of the License, or (at your
** option) any later version.
******************************************************************/
#include "particle_generator.h"

ParticleGenerator::ParticleGenerator(Shader shader, Texture2D texture, GLuint amount)
	: shader(shader), texture(texture), amount(amount)
{
	this->init();
}

void ParticleGenerator::Update(GLfloat dt, GLuint newParticles, glm::vec3 centerPos)
{
	//std::cout <<"ParticleGenerator::Update "<< dt << std::endl;
	// Add new particles 
	for (GLuint i = 0; i < newParticles; ++i)
	{
		int unusedParticle = this->firstUnusedParticle();
		this->respawnParticle(this->particles[unusedParticle], centerPos);
	}
	// Update all particles
	for (GLuint i = 0; i < this->amount; ++i)
	{
		Particle &p = this->particles[i];
		p.Life -= dt; // reduce life
		if (p.Life > 0.0f)
		{	// particle is alive, thus update
			p.Position += p.Velocity * dt;
			p.Color.a -= dt * 2.5;
			GLfloat distance = glm::length(p.Position - centerPos);
			if (distance > 2.0f)
			{
				p.Visible = GL_TRUE;
			}
		}
	}
}

// Render all particles
void ParticleGenerator::Draw()
{
	// Use additive blending to give it a 'glow' effect
	glBlendFunc(GL_SRC_ALPHA, GL_ONE);
	//glDisable(GL_DEPTH_TEST);
	this->shader.Use();
	for (Particle particle : this->particles)
	{
		if ( (particle.Life > 0.0f) && particle.Visible)
		{
			glm::mat4 model;
			model = glm::translate(model, particle.Position);
			model = glm::scale(model, glm::vec3(0.001f));
			this->shader.SetMatrix4("model", model);
			this->shader.SetVector4f("color", particle.Color);
			//this->texture.Bind();
			glBindVertexArray(this->VAO);
			glDrawElements(GL_TRIANGLE_STRIP, this->indexCount, GL_UNSIGNED_INT, 0);
			glBindVertexArray(0);
		}
	}
	// Don't forget to reset to default blending mode
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	//glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	//glm::mat4 model;
	//model = glm::translate(model, glm::vec3(0.0f, 0.0f, 0.0f));
	//model = glm::scale(model, glm::vec3(0.5f));
	//this->shader.SetMatrix4("model", model);
	//this->shader.SetVector4f("color", glm::vec4(0.0f, 1.0f, 0.0f, 1.0f));
	////this->texture.Bind();
	//glBindVertexArray(this->VAO);
	//glDrawElements(GL_TRIANGLE_STRIP, this->indexCount, GL_UNSIGNED_INT, 0);
	//glBindVertexArray(0);
	//glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	//glEnable(GL_DEPTH_TEST);
}

void ParticleGenerator::init()
{
	// Set up mesh and attribute properties
	GLuint vbo, ebo;
	glGenVertexArrays(1, &this->VAO);
	glGenBuffers(1, &vbo);
	glGenBuffers(1, &ebo);

	std::vector<glm::vec3> positions;
	std::vector<unsigned int> indices;

	const unsigned int X_SEGMENTS = 32;
	const unsigned int Y_SEGMENTS = 32;
	const float PI = 3.14159265359;
	for (unsigned int y = 0; y <= Y_SEGMENTS; ++y)
	{
		for (unsigned int x = 0; x <= X_SEGMENTS; ++x)
		{
			float xSegment = (float)x / (float)X_SEGMENTS;
			float ySegment = (float)y / (float)Y_SEGMENTS;
			float xPos = std::cos(xSegment * 2.0f * PI) * std::sin(ySegment * PI);
			float yPos = std::cos(ySegment * PI);
			float zPos = std::sin(xSegment * 2.0f * PI) * std::sin(ySegment * PI);

			positions.push_back(glm::vec3(xPos, yPos, zPos));
		}
	}
	bool oddRow = false;
	for (int y = 0; y < Y_SEGMENTS; ++y)
	{
		if (!oddRow) // even rows: y == 0, y == 2; and so on
		{
			for (int x = 0; x <= X_SEGMENTS; ++x)
			{
				indices.push_back(y       * (X_SEGMENTS + 1) + x);
				indices.push_back((y + 1) * (X_SEGMENTS + 1) + x);
			}
		}
		else
		{
			for (int x = X_SEGMENTS; x >= 0; --x)
			{
				indices.push_back((y + 1) * (X_SEGMENTS + 1) + x);
				indices.push_back(y       * (X_SEGMENTS + 1) + x);
			}
		}
		oddRow = !oddRow;
	}
	indexCount = indices.size();

	std::vector<float> data;
	for (int i = 0; i < positions.size(); ++i)
	{
		data.push_back(positions[i].x);
		data.push_back(positions[i].y);
		data.push_back(positions[i].z);
	}
	glBindVertexArray(VAO);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBufferData(GL_ARRAY_BUFFER, data.size() * sizeof(float), &data[0], GL_STATIC_DRAW);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), &indices[0], GL_STATIC_DRAW);
	float stride = (3) * sizeof(float);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, stride, (void*)0);


	
	glBindVertexArray(0);

	// Create this->amount default particle instances
	for (GLuint i = 0; i < this->amount; ++i)
		this->particles.push_back(Particle());
}

// Stores the index of the last particle used (for quick access to next dead particle)
GLuint lastUsedParticle = 0;
GLuint ParticleGenerator::firstUnusedParticle()
{
	// First search from last used particle, this will usually return almost instantly
	for (GLuint i = lastUsedParticle; i < this->amount; ++i) {
		if (this->particles[i].Life <= 0.0f) {
			lastUsedParticle = i;
			return i;
		}
	}
	// Otherwise, do a linear search
	for (GLuint i = 0; i < lastUsedParticle; ++i) {
		if (this->particles[i].Life <= 0.0f) {
			lastUsedParticle = i;
			return i;
		}
	}
	// All particles are taken, override the first one (note that if it repeatedly hits this case, more particles should be reserved)
	lastUsedParticle = 0;
	return 0;
}

void ParticleGenerator::respawnParticle(Particle &particle, glm::vec3 centerPos)
{
	particle.Position = centerPos;
	particle.Color = RED_COLOR;
	particle.Life = 10.0f;
	particle.Visible = GL_FALSE;
	particle.Velocity = glm::vec3((rand() % 100) / 100.0f - 0.5f, (rand() % 100) / 100.0f - 0.5f, (rand() % 100) / 100.0f - 0.5f);
	particle.Acceleration = glm::vec3(0.0f);
	//particle.Velocity = randVelocity * 50.0f;
}