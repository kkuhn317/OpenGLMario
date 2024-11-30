#pragma once
#ifndef FIREBALLOBJECT_H
#define FIREBALLOBJECT_H

#include <glad/glad.h>
#include <glm/glm.hpp>

#include "game_object.h"
#include "texture.h"

class FireballObject : public GameObject
{
public:
    // player state
    bool    Grounded;
    int     walkFrame = 0;
    const float walkFrameDistance = 10.0f; // Distance to move before changing walk frame
    float   currentWalkFrameDistance = 0.0f; // Distance moved since last frame change
    float   shootAnimLength = 1.0f; // Length of shoot animation
    float   currentShootAnimLength = 0.0f; // Time since shoot animation started

    // Sprites
    Texture2D spriteIdle;
    Texture2D spriteJump;
    Texture2D spriteWalk1;
    Texture2D spriteWalk2;
    Texture2D spriteWalk3;
    Texture2D spriteShoot;

    // constructor(s)
    PlayerObject(std::string spriteBase);
    PlayerObject(glm::vec2 pos, glm::vec2 size, std::string spriteBase);
    // moves the player
    glm::vec2 Move(bool holdingLeft, bool holdingRight, bool pressedJump, float dt, unsigned int window_width);
    // Makes the player shoot a fireball
    void Shoot();
    // resets the player to original state with given position and velocity
    void Reset(glm::vec2 position, glm::vec2 velocity);
};

#endif