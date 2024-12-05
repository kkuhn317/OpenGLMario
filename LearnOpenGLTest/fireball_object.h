#pragma once
#ifndef FIREBALLOBJECT_H
#define FIREBALLOBJECT_H

#include <glad/glad.h>
#include <glm/glm.hpp>
#include "texture.h"
#include "game_object.h"

class FireballObject : public GameObject
{
public:
    // constructor(s)
    FireballObject();
    FireballObject(glm::vec2 pos, glm::vec2 size, Texture2D sprite);
	// moves the fireball
    glm::vec2 Move(float dt, unsigned int groundLevel);
};

#endif