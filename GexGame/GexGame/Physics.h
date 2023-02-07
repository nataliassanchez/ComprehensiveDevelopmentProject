#pragma once

#include "Common.h"
#include "Entity.h"

namespace Physics
{
	sf::Vector2f getOverlap(std::shared_ptr<Entity> a, std::shared_ptr<Entity> b);
	sf::Vector2f getPreviousOverlap(std::shared_ptr<Entity> a, std::shared_ptr<Entity> b);
};
