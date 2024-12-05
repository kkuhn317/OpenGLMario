/*******************************************************************
** This code is part of Breakout (modified by Kevin Kuhn)
**
** Breakout is free software: you can redistribute it and/or modify
** it under the terms of the CC BY 4.0 license as published by
** Creative Commons, either version 4 of the License, or (at your
** option) any later version.
******************************************************************/
#include "game.h"
#include "resource_manager.h"
#include "sprite_renderer.h"
#include "game_object.h"
#include "player_object.h"

// Game-related State data
SpriteRenderer* Renderer;
PlayerObject* Mario;
PlayerObject* Luigi;
glm::vec2 CameraPosition = glm::vec2(0.0f, 0.0f);
glm::vec2 CameraSize = glm::vec2(800.0f, 600.0f);

Game::Game(unsigned int width, unsigned int height)
    : State(GAME_ACTIVE), Keys(), Width(width), Height(height)
{

}

Game::~Game()
{
    delete Renderer;
    delete Mario;
	delete Luigi;
}

void Game::Init()
{
    // load shaders
    ResourceManager::LoadShader("shaders/sprite.vs", "shaders/sprite.frag", nullptr, "sprite");
    // configure shaders
    glm::mat4 projection = glm::ortho(0.0f, static_cast<float>(this->Width),
        static_cast<float>(this->Height), 0.0f, -1.0f, 1.0f);
	CameraSize = glm::vec2(this->Width, this->Height);

	// Initialize view matrix (camera position)
	glm::mat4 view = glm::mat4(1.0f); // Identity matrix to start with
    // Apply initial camera translation, e.g., to start the camera centered on the screen
    view = glm::translate(view, glm::vec3(0.0f, -300.0f, 0.0f)); // Set the camera position here

    ResourceManager::GetShader("sprite").Use().SetInteger("image", 0);
    ResourceManager::GetShader("sprite").SetMatrix4("projection", projection);
	ResourceManager::GetShader("sprite").SetMatrix4("view", view);
    
    // set render-specific controls
    //Renderer = new SpriteRenderer(ResourceManager::GetShader("sprite"));
    Shader myShader;
    myShader = ResourceManager::GetShader("sprite");
    Renderer = new SpriteRenderer(myShader);

    // load textures
    ResourceManager::LoadTexture("textures/background.png", false, "background");
	ResourceManager::LoadTexture("textures/hills.png", true, "hills");
    ResourceManager::LoadTexture("textures/ground.png", false, "ground");
    ResourceManager::LoadTexture("textures/players/fireball.png", true, "fireball");
	ResourceManager::LoadTexture("textures/healthbar.png", true, "healthbar");
	ResourceManager::LoadTexture("textures/health.png", false, "health");
	ResourceManager::LoadTexture("textures/mariowin.png", true, "mariowin");
	ResourceManager::LoadTexture("textures/luigiwin.png", true, "luigiwin");

    // configure Players
    glm::vec2 marioPos = glm::vec2(0, this->Height - PLAYER_SIZE.y - GROUND_HEIGHT);
    Mario = new PlayerObject(marioPos, PLAYER_SIZE, "textures/players/mario");
	glm::vec2 luigiPos = glm::vec2(500, this->Height - PLAYER_SIZE.y - GROUND_HEIGHT);
	Luigi = new PlayerObject(luigiPos, PLAYER_SIZE, "textures/players/luigi");
}

void Game::Update(float dt)
{
    if (this->State == GAME_ACTIVE)
    {
        // Calculate the center between Mario and Luigi
		glm::vec2 marioCenter = Mario->Position + Mario->Size / 2.0f;
		glm::vec2 luigiCenter = Luigi->Position + Luigi->Size / 2.0f;
        glm::vec2 playersCenter = (marioCenter + luigiCenter) / 2.0f;

        // Calculate the distance between Mario and Luigi
        float distance = glm::length(Mario->Position - Luigi->Position);

        // Set the zoom factor based on distance (ensure the zoom stays within limits)
        float zoomFactor = (distance / 500.0f);
		if (zoomFactor < 0.75f)
			zoomFactor = 0.75f;

        // Calculate the camera position, keeping it centered between Mario and Luigi
        glm::vec3 newCameraPosition(playersCenter.x, playersCenter.y, 0.0f);

		// cameraPosition is the center of the screen, so we need to offset it by half the screen size to get the top-left corner
		newCameraPosition.x -= this->Width * zoomFactor / 2.0f;
        newCameraPosition.y = this->Height - this->Height * zoomFactor;    // Lock the bottom of the camera to the bottom of the level

		// Update CameraPosition
		CameraPosition = newCameraPosition;

        // Update CameraSize
		CameraSize = glm::vec2(this->Width * zoomFactor, this->Height * zoomFactor);

		// Move fireballs
		Mario->MoveFireballs(dt, this->Height - GROUND_HEIGHT);
		Luigi->MoveFireballs(dt, this->Height - GROUND_HEIGHT);

		// Check for collisions
		DoCollisions();

		// Check for Mario win
		if (Luigi->Health <= 0.0f)
		{
			this->State = GAME_MARIO_WIN;
		}
		if (Mario->Health <= 0.0f)
		{
			this->State = GAME_LUIGI_WIN;
		}
    }
}

bool MjumpLastFrame = false;
bool LjumpLastFrame = false;
bool MshootLastFrame = false;
bool LshootLastFrame = false;

void Game::ProcessInput(float dt)
{
    if (this->State == GAME_ACTIVE)
    {
        bool MleftPressed = this->Keys[GLFW_KEY_A];
		bool MrightPressed = this->Keys[GLFW_KEY_D];
		bool Mjump = this->Keys[GLFW_KEY_W];
		bool MjumpPressed = Mjump && !MjumpLastFrame; // only jump if the key was not pressed last frame
		MjumpLastFrame = Mjump;
		Mario->Move(MleftPressed, MrightPressed, MjumpPressed, dt, this->Height - GROUND_HEIGHT);

		bool LleftPressed = this->Keys[GLFW_KEY_LEFT];
		bool LrightPressed = this->Keys[GLFW_KEY_RIGHT];
		bool Ljump = this->Keys[GLFW_KEY_UP];
		bool LjumpPressed = Ljump && !LjumpLastFrame; // only jump if the key was not pressed last frame
		LjumpLastFrame = Ljump;
		Luigi->Move(LleftPressed, LrightPressed, LjumpPressed, dt, this->Height - GROUND_HEIGHT);

		// Shoot fireball
		bool Mshoot = this->Keys[GLFW_KEY_S];
		bool MshootPressed = Mshoot && !MshootLastFrame; // only shoot if the key was not pressed last frame
		MshootLastFrame = Mshoot;

		bool Lshoot = this->Keys[GLFW_KEY_DOWN];
		bool LshootPressed = Lshoot && !LshootLastFrame; // only shoot if the key was not pressed last frame
		LshootLastFrame = Lshoot;

        if (MshootPressed) {
            Mario->Shoot();
        }
        if (LshootPressed) {
            Luigi->Shoot();
        }
	}
	else if (this->State == GAME_MARIO_WIN || this->State == GAME_LUIGI_WIN)
	{
		if (this->Keys[GLFW_KEY_SPACE])
		{
			this->State = GAME_ACTIVE;
			ResetPlayer();
		}
	}
}

void Game::Render()
{

	// Set the view and projection matrices in the shader
	// use CameraPosition as the top-left corner of the camera and CameraSize as the size of the camera
	// View matrix
	ResourceManager::GetShader("sprite").Use();
    glm::mat4 view = glm::mat4(1.0f);  // Start with identity matrix
    view = glm::translate(view, glm::vec3(-CameraPosition.x, -CameraPosition.y, 0.0f)); // Translate to camera position
    ResourceManager::GetShader("sprite").SetMatrix4("view", view); 
	// Projection matrix
    glm::mat4 projection = glm::ortho(0.0f, CameraSize.x, CameraSize.y, 0.0f, -1.0f, 1.0f);
	ResourceManager::GetShader("sprite").SetMatrix4("projection", projection);

	// get backround and ground textures
	Texture2D backgroundTexture = ResourceManager::GetTexture("background");
	Texture2D hillsTexture = ResourceManager::GetTexture("hills");
    Texture2D groundTexture = ResourceManager::GetTexture("ground");

	glm::vec2 backgroundSize = glm::vec2(backgroundTexture.Width, backgroundTexture.Height) * 2.0f;
	glm::vec2 hillsSize = glm::vec2(hillsTexture.Width, hillsTexture.Height) * 2.0f;
	glm::vec2 groundSize = glm::vec2(groundTexture.Width, groundTexture.Height) * 2.0f;

	// Fake scrolling using texture offsets
	glm::vec2 backgroundOffset = glm::vec2(CameraPosition.x, CameraPosition.y - this->Height + backgroundSize.y);
	glm::vec2 hillsOffset = glm::vec2(CameraPosition.x, 0);
    glm::vec2 groundOffset = glm::vec2(CameraPosition.x, 0);

	// Draw background
	Renderer->DrawRepeatingSprite(backgroundTexture, CameraPosition, backgroundSize, CameraSize, backgroundOffset);

	// Draw hills
	Renderer->DrawRepeatingSprite(hillsTexture, glm::vec2(CameraPosition.x, this->Height - hillsSize.y), hillsSize, glm::vec2(CameraSize.x, hillsSize.y), hillsOffset);

	// Draw ground
	Renderer->DrawRepeatingSprite(groundTexture, glm::vec2(CameraPosition.x, this->Height - GROUND_HEIGHT), groundSize, glm::vec2(CameraSize.x, GROUND_HEIGHT), groundOffset);

    // draw players
    Mario->Draw(*Renderer);
	Luigi->Draw(*Renderer);

	// draw fireballs
	Mario->DrawFireballs(*Renderer);
	Luigi->DrawFireballs(*Renderer);

	// Set view and projection matrices in the shader
	// So the UI stays in the same place on the screen
	ResourceManager::GetShader("sprite").Use();
	ResourceManager::GetShader("sprite").SetMatrix4("view", glm::mat4(1.0f));
	projection = glm::ortho(0.0f, static_cast<float>(this->Width), static_cast<float>(this->Height), 0.0f, -1.0f, 1.0f);
	ResourceManager::GetShader("sprite").SetMatrix4("projection", projection);

	// Draw health bars
	Texture2D healthbarTexture = ResourceManager::GetTexture("healthbar");
	Texture2D healthTexture = ResourceManager::GetTexture("health");

	glm::vec2 healthBarSize = glm::vec2(200.0f, 40.0f); // Double size of health bar
	glm::vec2 mHealthBarPos = glm::vec2(32.0f, 32.0f);
	glm::vec2 lHealthBarPos = glm::vec2(this->Width - 32.0f - healthBarSize.x, 32.0f);
	float healthBarBorder = 4.0f;
	glm::vec2 mHealthSize = glm::vec2(Mario->Health * (healthBarSize.x - 2.0f * healthBarBorder) / 100.0f, healthBarSize.y - 2.0f * healthBarBorder);
	glm::vec2 lHealthSize = glm::vec2(Luigi->Health * (healthBarSize.x - 2.0f * healthBarBorder) / 100.0f, healthBarSize.y - 2.0f * healthBarBorder);

	// Draw Mario's health bar
	Renderer->DrawSprite(healthbarTexture, mHealthBarPos, healthBarSize, 0.0f);
	Renderer->DrawSprite(healthTexture, mHealthBarPos + glm::vec2(healthBarBorder), mHealthSize, 0.0f);

	// Draw Luigi's health bar
	Renderer->DrawSprite(healthbarTexture, lHealthBarPos, healthBarSize, 0.0f);
	Renderer->DrawSprite(healthTexture, lHealthBarPos + glm::vec2(healthBarBorder), lHealthSize, 0.0f);

	// Draw win screen
	if (this->State == GAME_MARIO_WIN)
	{
		Texture2D marioWinTexture = ResourceManager::GetTexture("mariowin");
		Renderer->DrawSprite(marioWinTexture, glm::vec2(0,0), glm::vec2(this->Width, this->Height), 0.0f);
	}
	else if (this->State == GAME_LUIGI_WIN)
	{
		Texture2D luigiWinTexture = ResourceManager::GetTexture("luigiwin");
		Renderer->DrawSprite(luigiWinTexture, glm::vec2(0, 0), glm::vec2(this->Width, this->Height), 0.0f);
	}

}

void Game::ResetLevel()
{

}

void Game::ResetPlayer()
{
    // reset player positions
	Mario->Position = glm::vec2(0, this->Height - PLAYER_SIZE.y - GROUND_HEIGHT);
	Luigi->Position = glm::vec2(500, this->Height - PLAYER_SIZE.y - GROUND_HEIGHT);
	// reset player health
	Mario->Health = 100.0f;
	Luigi->Health = 100.0f;
	// reset fireballs
	Mario->fireballs.clear();
	Luigi->fireballs.clear();
}

// collision detection
bool CheckCollision(GameObject& one, GameObject& two);

void Game::DoCollisions()
{
	// For each fireball, check for collisions with the players
	for (unsigned int i = 0; i < Mario->fireballs.size(); i++)
	{
		// Check for collision with Luigi
		bool collided = CheckCollision(Mario->fireballs[i], *Luigi);
		if (collided)
		{
			// Reduce Luigi's health
			Luigi->Health -= FIREBALL_DAMAGE;
			// Remove the fireball
			Mario->RemoveFireball(i);
			i--; // Decrement i to avoid skipping the next fireball
		}
	}

    for (unsigned int i = 0; i < Luigi->fireballs.size(); i++)
    {
        // Check for collision with Mario
        bool collided = CheckCollision(Luigi->fireballs[i], *Mario);
        if (collided)
        {
            // Reduce Mario's health
			Mario->Health -= FIREBALL_DAMAGE;
            // Remove the fireball
            Luigi->RemoveFireball(i);
            i--; // Decrement i to avoid skipping the next fireball
        }

    }

}

bool CheckCollision(GameObject& one, GameObject& two) // AABB - AABB collision
{
    // collision x-axis?
    bool collisionX = one.Position.x + one.Size.x >= two.Position.x &&
        two.Position.x + two.Size.x >= one.Position.x;
    // collision y-axis?
    bool collisionY = one.Position.y + one.Size.y >= two.Position.y &&
        two.Position.y + two.Size.y >= one.Position.y;
    // collision only if on both axes
    return collisionX && collisionY;
}