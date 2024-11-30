#pragma once
#ifndef PLAYEROBJECT_H
#define PLAYEROBJECT_H

#include <glad/glad.h>
#include <glm/glm.hpp>

#include "game_object.h"
#include "fireball_object.h"
#include "texture.h"
#include <vector>

class PlayerObject : public GameObject
{
public:
    // player state
    bool    Grounded;
    int     walkFrame = 0;
	const float walkFrameDistance = 10.0f; // Distance to move before changing walk frame
	float   currentWalkFrameDistance = 0.0f; // Distance moved since last frame change
	float   shootAnimLength = 0.2f; // Length of shoot animation
	float   currentShootAnimLength = 0.0f; // Time since shoot animation started
	float	Health = 100.0f;

    // Sprites
	Texture2D spriteIdle;
	Texture2D spriteJump;
    Texture2D spriteWalk1;
	Texture2D spriteWalk2;
	Texture2D spriteWalk3;
    Texture2D spriteShoot;
	Texture2D spriteFireball;

    // Fireballs
	std::vector<FireballObject> fireballs;
	std::vector<float> fireballLifeTimes;

    // constructor(s)
    PlayerObject(std::string spriteBase);
    PlayerObject(glm::vec2 pos, glm::vec2 size, std::string spriteBase);
    // moves the player
    glm::vec2 Move(bool holdingLeft, bool holdingRight, bool pressedJump, float dt, unsigned int window_width);
	// Makes the player shoot a fireball
	void Shoot();
	// moves the fireballs
	void MoveFireballs(float dt, unsigned int window_width);
    // renders the fireballs
	void DrawFireballs(SpriteRenderer& renderer);
	// removes a fireball from the list
	void RemoveFireball(unsigned int index);
    // resets the player to original state with given position and velocity
    void Reset(glm::vec2 position, glm::vec2 velocity);
};

#endif