#include "SpriteRenderer.h"
#include "DerpEngine.h"
#include <iostream>

void SpriteRenderer::start() {
	sf::Vector2f calculated_origin;
	calculated_origin.x = this->sprite.getLocalBounds().left + (this->sprite.getLocalBounds().width / 2);
	calculated_origin.y = this->sprite.getLocalBounds().top + (this->sprite.getLocalBounds().height / 2);
	this->sprite.setOrigin(calculated_origin);
}

void SpriteRenderer::update(float delta_time) {
	DerpEngine::render_window->draw(this->sprite, this->game_object->get_world_transform());
}

void SpriteRenderer::set_sprite(std::string image_path) {
	this->sprite_texture.loadFromFile(image_path);
	this->sprite.setTexture(this->sprite_texture);
}