#include "PhyisicsEngine.h"
#include <cmath>
#include <algorithm>
#include <numeric>
#include <SFML\System\Vector2.hpp>
#include <iostream>


PhysicsEngine* PhysicsEngine::phyisics_engine_instance;

PhysicsEngine::PhysicsEngine(){
	PhysicsEngine::phyisics_engine_instance = this;
}

void PhysicsEngine::add_rigidBody(Rigidbody* object_rigidbody){
	rigidbodies_list.push_back(object_rigidbody);
}

void PhysicsEngine::update_gameobjects_phyisics(float delta_time)
{
	for each (Rigidbody *rigidbody in rigidbodies_list)
	{
		rigidbody->update_phyisics(delta_time);
	}
}

bool PhysicsEngine::is_grounded(Rigidbody* object_rigidbody)
{
	for each (Rigidbody *rigidbody in rigidbodies_list)
	{
		if (rigidbody != object_rigidbody)
		{
			if (object_rigidbody->aabb.bottom_left.x < rigidbody->aabb.top_right.x
				&& object_rigidbody->aabb.top_right.x > rigidbody->aabb.bottom_left.x
				&& abs(object_rigidbody->aabb.bottom_left.y - rigidbody->aabb.top_right.y) <= groundedTol)
			{
				if (abs(object_rigidbody->current_velocity.y) < groundedTol)
					return true;
			}
		}
	}
	return false;
}

void PhysicsEngine::check_collisions()
{
	for each (Rigidbody *rigidbody_a in rigidbodies_list) {
		for each (Rigidbody *rigidbody_b in rigidbodies_list) {
			if (rigidbody_a == rigidbody_b)
				continue;
			CollisionPair pair;
			CollisionInfo info;

			pair.object_rigidbody_a = rigidbody_a;
			pair.object_rigidbody_b = rigidbody_b;

			sf::Vector2f distance;

			sf::Vector2f halfSizeA = (rigidbody_a->aabb.top_right - rigidbody_a->aabb.bottom_left) * 0.5f;
			sf::Vector2f halfSizeB = (rigidbody_b->aabb.top_right - rigidbody_b->aabb.bottom_left) * 0.5f;

			sf::Vector2f gap = sf::Vector2f(abs(distance.x), abs(distance.y)) - (halfSizeA + halfSizeB);

			std::map<CollisionPair*, CollisionInfo*>::iterator it = collision_list.find(&pair);

			if (gap.x < 0 && gap.y < 0) {

				if (it != collision_list.end()) {
					collision_list.erase(&pair);
				}

				if (gap.x > gap.y) {
					if (distance.x > 0) {
						// ... Update collision normal
						info.collisonNormal = sf::Vector2f(1, 0);
					}
					else {
						// ... Update collision normal
						info.collisonNormal = sf::Vector2f(-1, 0);
					}
					info.penatration = gap.x;
					std::cout << "COLLISION ON X-AXIS" << std::endl;
				}
				else {
					if (distance.y > 0) {
						// ... Update collision normal
						info.collisonNormal = sf::Vector2f(0, 1);
					}
					else {
						// ... Update collision normal
						info.collisonNormal = sf::Vector2f(0, -1);
					}
					info.penatration = gap.y;
					std::cout << "COLLISION ON Y-AXIS" << std::endl;
				}
				collision_list.insert(std::pair<CollisionPair*, CollisionInfo*>(&pair, &info));
			}
			else if (it != collision_list.end()) {
				collision_list.erase(&pair);
			}
		}
	}
}

void PhysicsEngine::resolve_collisons()
{
	for (std::map<CollisionPair*, CollisionInfo*>::iterator it = collision_list.begin(); it !=  collision_list.end(); it++)
	{
		float min_bounce = std::min(it->first->object_rigidbody_a->bounciness, it->first->object_rigidbody_b->bounciness);

		sf::Vector2f temp = it->first->object_rigidbody_b->current_velocity - it->first->object_rigidbody_a->current_velocity;
		float vel_along_normal = (temp.x * it->second->collisonNormal.y) + (temp.y * it->second->collisonNormal.y);

		if (vel_along_normal > 0)
			continue;

		float j = -(1 + min_bounce) * vel_along_normal;

		float inv_mass_a, inv_mass_b;

		//Handle inv mass a
		if (it->first->object_rigidbody_a->mass == 0)
			inv_mass_a = 0;
		else
			inv_mass_a = 1 / it->first->object_rigidbody_a->mass;

		//Handle inv mass b
		if (it->first->object_rigidbody_b->mass == 0)
			inv_mass_b = 0;
		else
			inv_mass_b = 1 / it->first->object_rigidbody_b->mass;

		j /= inv_mass_a + inv_mass_b;

		sf::Vector2f impulse = j * it->second->collisonNormal;

		if (it->first->object_rigidbody_a->mass != 0)
			it->first->object_rigidbody_a->current_velocity -= 1 / it->first->object_rigidbody_a->mass * impulse;

		if (it->first->object_rigidbody_b->mass != 0)
			it->first->object_rigidbody_b->current_velocity -= 1 / it->first->object_rigidbody_b->mass * impulse;

		if (abs(it->second->penatration) > 0.01f)
			correct_positions(it->first);
	} 
}

void PhysicsEngine::correct_positions(CollisionPair *pair)
{
	const float percent = 0.2f;

	float inv_mass_a, inv_mass_b;

	//Handle inv mass a
	if (pair->object_rigidbody_a->mass == 0)
		inv_mass_a = 0;
	else
		inv_mass_a = 1 / pair->object_rigidbody_a->mass;

	//Handle inv mass b
	if (pair->object_rigidbody_b->mass == 0)
		inv_mass_b = 0;
	else
		inv_mass_b = 1 / pair->object_rigidbody_b->mass;

	sf::Vector2f correction = ((collision_list.find(pair)->second->penatration / (inv_mass_a + inv_mass_b)) * percent) * -collision_list.find(pair)->second->collisonNormal;

	sf::Vector2f temp = pair->object_rigidbody_a->game_object->transform->getPosition();
	temp -= inv_mass_a * correction;
	pair->object_rigidbody_a->game_object->transform->setPosition(temp);
	
	temp = sf::Vector2f(pair->object_rigidbody_b->game_object->transform->getPosition());
	temp += inv_mass_b * correction;
	pair->object_rigidbody_b->game_object->transform->setPosition(temp);
}

void PhysicsEngine::update_phyisics(float delta_time)
{
	update_gameobjects_phyisics(delta_time);
	check_collisions();
	resolve_collisons();
}