#include "Bullet.h"

Bullet::Bullet()
{
}

Bullet::Bullet(Texture* texture, float posX, float posY, float dirX, float dirY, float movementSpeed)
{
	this->shape.setTexture(*texture);
	this->shape.setPosition(posX, posY);

	this->direction.x = dirX;
	this->direction.y = dirY;
	this->movementSpeed = movementSpeed;
}

Bullet::~Bullet()
{
}

const FloatRect Bullet::getBounds() const
{
	return this->shape.getGlobalBounds();
}

void Bullet::update()
{
	//Movement
	this->shape.move(this->movementSpeed * this->direction);
}

void Bullet::render(RenderTarget * target)
{
	target->draw(this->shape);
}
