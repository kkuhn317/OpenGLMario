#include "player_object.h"
#include "resource_manager.h"
#include <string>
#include "fireball_object.h"


PlayerObject::PlayerObject(std::string spriteBase)
	: GameObject(), Grounded(true)
{ 
    this->spriteIdle = ResourceManager::LoadTexture((spriteBase + "_idle.png").c_str(), true, spriteBase + "_idle");
	this->spriteJump = ResourceManager::LoadTexture((spriteBase + "_jump.png").c_str(), true, spriteBase + "_jump");
	this->spriteWalk1 = ResourceManager::LoadTexture((spriteBase + "_walk1.png").c_str(), true, spriteBase + "_walk1");
	this->spriteWalk2 = ResourceManager::LoadTexture((spriteBase + "_walk2.png").c_str(), true, spriteBase + "_walk2");
	this->spriteWalk3 = ResourceManager::LoadTexture((spriteBase + "_walk3.png").c_str(), true, spriteBase + "_walk3");
	this->spriteShoot = ResourceManager::LoadTexture((spriteBase + "_shoot.png").c_str(), true, spriteBase + "_shoot");
	this->spriteFireball = ResourceManager::LoadTexture((spriteBase + "_fireball.png").c_str(), true, spriteBase + "_fireball");
}

PlayerObject::PlayerObject(glm::vec2 pos, glm::vec2 size, std::string spriteBase)
    : GameObject(pos, size, spriteIdle, glm::vec3(1.0f)), Grounded(true)
{
    this->spriteIdle = ResourceManager::LoadTexture((spriteBase + "_idle.png").c_str(), true, spriteBase + "_idle");
    this->spriteJump = ResourceManager::LoadTexture((spriteBase + "_jump.png").c_str(), true, spriteBase + "_jump");
	this->spriteWalk1 = ResourceManager::LoadTexture((spriteBase + "_walk1.png").c_str(), true, spriteBase + "_walk1");
	this->spriteWalk2 = ResourceManager::LoadTexture((spriteBase + "_walk2.png").c_str(), true, spriteBase + "_walk2");
	this->spriteWalk3 = ResourceManager::LoadTexture((spriteBase + "_walk3.png").c_str(), true, spriteBase + "_walk3");
	this->spriteShoot = ResourceManager::LoadTexture((spriteBase + "_shoot.png").c_str(), true, spriteBase + "_shoot");
	this->spriteFireball = ResourceManager::LoadTexture((spriteBase + "_fireball.png").c_str(), true, spriteBase + "_fireball");

	printf("full path: %s\n", (spriteBase + "_idle.png").c_str());

}

glm::vec2 PlayerObject::Move(bool holdingLeft, bool holdingRight, bool pressedJump, float dt, unsigned int groundLevel)
{
    float maxSpeed = 300.0f;

    // move left/right
    if (holdingLeft)
    {
		this->Velocity.x -= 400.0f * dt;
    }
    else if (holdingRight)
    {
		this->Velocity.x += 400.0f * dt;
    }
    else {
    // slow down the player if no keys are pressed
        if (this->Velocity.x > 0.0f)
        {
            this->Velocity.x -= 300.0f * dt;
            if (this->Velocity.x < 0.0f) this->Velocity.x = 0.0f;
        }
        else if (this->Velocity.x < 0.0f)
        {
            this->Velocity.x += 300.0f * dt;
            if (this->Velocity.x > 0.0f) this->Velocity.x = 0.0f;
        }
    }
    // cap the horizontal velocity to maxSpeed
    if (this->Velocity.x > maxSpeed)
    {
    this->Velocity.x = maxSpeed;
    }
    if (this->Velocity.x < -maxSpeed)
    {
    this->Velocity.x = -maxSpeed;
    }
	// jump
	if (pressedJump && this->Grounded)
	{
        this->Velocity.y = -300.0f;
		this->Grounded = false;
	}
    
	// move the player
	this->Position += this->Velocity * dt;
	// if below the floor, set grounded to true and snap vertical position to the floor
	if (this->Position.y >= groundLevel - this->Size.y)
	{
		this->Grounded = true;
		this->Position.y = groundLevel - this->Size.y;
		this->Velocity.y = 0.0f;
	}

	// gravity
	this->Velocity.y += 400.0f * dt;
	
    // Sprite
	if (this->Velocity.x > 0.0f) this->flipX = false;
	if (this->Velocity.x < 0.0f) this->flipX = true;


	if (this->currentShootAnimLength > 0.0f)
	{
		this->currentShootAnimLength -= dt;
		if (this->currentShootAnimLength > 0.0f)
		{
			this->Sprite = this->spriteShoot;
			return this->Position;
		}
	}

	if (this->Grounded)
	{
        if (this->Velocity.x != 0.0f) {
            
			this->currentWalkFrameDistance += glm::abs(this->Velocity.x) * dt;
			while (this->currentWalkFrameDistance >= this->walkFrameDistance)
			{
				this->currentWalkFrameDistance -= this->walkFrameDistance;
				this->walkFrame++;
				if (this->walkFrame > 2) this->walkFrame = 0;
			}
			switch (this->walkFrame)
			{
			case 0:
				this->Sprite = this->spriteWalk1;
				break;
			case 1:
				this->Sprite = this->spriteWalk2;
				break;
			case 2:
				this->Sprite = this->spriteWalk3;
				break;
			}
        }
        else {
            this->Sprite = this->spriteIdle;
        }
	}
	else
	{
		this->Sprite = this->spriteJump;
	}

    return this->Position;
}

void PlayerObject::Shoot()
{
	this->Sprite = this->spriteShoot;
	this->currentShootAnimLength = this->shootAnimLength;

	glm::vec2 fireballPos = this->Position + glm::vec2(flipX ? -10 : this->Size.x / 2 + 10, this->Size.y / 2 -10);
	glm::vec2 fireballSize = glm::vec2(16,16);

	// Create a new fireball object
	FireballObject fireball = FireballObject(fireballPos, fireballSize, spriteFireball);
	fireball.Velocity = glm::vec2(400.0f * (flipX ? -1 : 1), 200.0f);
	this->fireballs.push_back(fireball);
	this->fireballLifeTimes.push_back(2.0f);
}

// moves the fireballs
void PlayerObject::MoveFireballs(float dt, unsigned int window_width)
{
    for (int i = 0; i < this->fireballs.size(); i++)
    {
        this->fireballs[i].Move(dt, window_width);
        this->fireballLifeTimes[i] -= dt;
        if (this->fireballLifeTimes[i] <= 0.0f)
        {
            this->RemoveFireball(i);
            i--; // adjust index after removal
        }
    }
}

// renders the fireballs
void PlayerObject::DrawFireballs(SpriteRenderer& renderer)
{
	for (FireballObject& fireball : this->fireballs)
	{
		fireball.Draw(renderer);
	}
}

// removes a fireball from the list
void PlayerObject::RemoveFireball(unsigned int index)
{
	this->fireballs.erase(this->fireballs.begin() + index);
	this->fireballLifeTimes.erase(this->fireballLifeTimes.begin() + index);
}


// resets the player to initial position
void PlayerObject::Reset(glm::vec2 position, glm::vec2 velocity)
{
    this->Position = position;
    this->Velocity = velocity;
}