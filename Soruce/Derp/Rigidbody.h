#pragma once
#include <SFML\Graphics.hpp>
#include "Component.h"
#include "BoxCollider.hpp"
#include <TinyXML/tinyxml.h>
#include "Debug.hpp"
#include "GameObject.h"

class PhysicsEngine;

class Rigidbody : public Component {
public:

	static void set_component_from_xml(TiXmlElement* xml_element, GameObject* gameobject) {
		Rigidbody *rigidbody = new Rigidbody();
		rigidbody->mass = std::stof(xml_element->FirstChildElement()->GetText());
		rigidbody->bounciness = std::stof(xml_element->FirstChildElement()->NextSiblingElement()->GetText());
		gameobject->add_component(rigidbody);
	}

	float mass;
	float bounciness;

	//The force that will slow down objects velocity (1.0f is no force)
	const float linear_dampaning;
	//The threshold that will force the player to stop
	//moving if they pass this value
	const float stopping_speed;

	bool obeys_gravity = true;
	bool grounded;

	//const sf::Vector2f gravity = sf::Vector2f(0.0f, 0.0f);
	sf::Vector2f gravity = sf::Vector2f(0.0f, 0.00032f);
	const sf::Vector2f max_velocity = sf::Vector2f(0.5f, 0.5f);
	sf::Vector2f current_velocity;

	struct AABB
	{
		sf::Vector2f bottom_left;
		sf::Vector2f top_right;
	};

	AABB aabb;

public:
	Rigidbody();
	void add_force(sf::Vector2f force);
	void stop(bool stop_x_axis, bool stop_y_axis);
	bool is_grounded();
	//void update(float delta_time) override;
	void start() override;
	void update_phyisics(float delta_time);
	void set_aabb();
	void on_collision_enter(GameObject *colliding_gameobject);

private:
	sf::Vector2f total_forces;
	PhysicsEngine *engine;
	BoxCollider *box_collider;
};