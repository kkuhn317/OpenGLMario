#include "fireball_object.h"
#include "resource_manager.h"
#include "game_object.h"
#include <string>


FireballObject::FireballObject()
	: GameObject() {}

FireballObject::FireballObject(glm::vec2 pos, glm::vec2 size, Texture2D sprite)
	: GameObject(pos, size, sprite, glm::vec3(1.0f)) {}

glm::vec2 FireballObject::Move(float dt, unsigned int groundLevel)
{
	// move the fireball
	this->Position += this->Velocity * dt;

	// Rotate the fireball
	bool movingRight = this->Velocity.x > 0.0f;
	this->Rotation += 1000.0f * dt * (movingRight ? 1.0f : -1.0f);


	// if below the floor snap to floor and set velocity negative so it bounces
	if (this->Position.y >= groundLevel - this->Size.y)
	{
		this->Position.y = groundLevel - this->Size.y;
		this->Velocity.y = -200.0f;
	}

	// gravity
	this->Velocity.y += 600.0f * dt;

	// Sprite

	return this->Position;
}