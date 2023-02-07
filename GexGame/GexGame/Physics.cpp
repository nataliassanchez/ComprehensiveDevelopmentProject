
#include "Physics.h"
#include <cmath>

sf::Vector2f Physics::getOverlap(std::shared_ptr<Entity> a, std::shared_ptr<Entity> b)
{

    auto atx = a->getComponent<CTransform>();
    auto abb = a->getComponent<CBoundingBox>();
    auto btx = b->getComponent<CTransform>();
    auto bbb = b->getComponent<CBoundingBox>();

    sf::Vector2f  overlap(0.f, 0.f);
    if (abb.has && bbb.has)
    {
        float dx = std::abs(atx.pos.x - btx.pos.x);
        float dy = std::abs(atx.pos.y - btx.pos.y);
        overlap = sf::Vector2f(abb.halfSize.x + bbb.halfSize.x - dx, abb.halfSize.y + bbb.halfSize.y - dy);
    }
    return overlap;

}

sf::Vector2f  Physics::getPreviousOverlap(std::shared_ptr<Entity> a, std::shared_ptr<Entity> b)
{
    auto atx = a->getComponent<CTransform>();
    auto abb = a->getComponent<CBoundingBox>();
    auto btx = b->getComponent<CTransform>();
    auto bbb = b->getComponent<CBoundingBox>();

    sf::Vector2f  overlap(0.f, 0.f); // no AABB, no overlapp return (0,0) 
    if (abb.has && bbb.has)
    {
        float dx = std::abs(atx.prevPos.x - btx.prevPos.x);
        float dy = std::abs(atx.prevPos.y - btx.prevPos.y);
        overlap = sf::Vector2f(abb.halfSize.x + bbb.halfSize.x - dx, abb.halfSize.y + bbb.halfSize.y - dy);
    }
    return overlap;
}

