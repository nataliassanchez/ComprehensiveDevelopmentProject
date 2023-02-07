//
// Created by David Burchill on 2022-09-26.
//

#ifndef SFMLCLASS_COMPONENTS_H
#define SFMLCLASS_COMPONENTS_H

#include <SFML/Graphics.hpp>
#include "Utilities.h"
#include "Animation.h"
#include <array>

struct Component
{
	bool has{ false };
	Component() = default;
};


struct CAnimation : public Component
{
	Animation animation;
	bool repeat{ false };
	CAnimation() = default;

	CAnimation(const Animation& animation, bool r)
		:animation(animation), repeat(r) {}
};

struct CInput : public Component
{
	bool up{ false };
	bool left{ false };
	bool right{ false };
	bool down{ false };

	CInput() = default;
};


struct CLifespan : public Component
{
	int total{ 0 };
	int remaining{ 0 };

	CLifespan() = default;
	CLifespan(int t) : total(t), remaining{ t } {}

};


struct CBoundingBox : public Component
{
	sf::Vector2f size{ 0.f, 0.f };
	sf::Vector2f halfSize{ 0.f, 0.f };

	CBoundingBox() = default;
	CBoundingBox(const sf::Vector2f& s) : size(s), halfSize(0.5f * s)
	{}
};


struct CTransform : public Component
{

	sf::Vector2f	pos{ 0.f, 0.f };
	sf::Vector2f	prevPos{ 0.f, 0.f };
	sf::Vector2f	scale{ 1.f, 1.f };
	sf::Vector2f	vel{ 0.f, 0.f };

	float   angVel{ 0 };
	float	angle{ 0.f };

	CTransform() = default;
	CTransform(const sf::Vector2f& p) : pos(p) {}
	CTransform(const sf::Vector2f& p, const sf::Vector2f& v, const sf::Vector2f& sc, float a)
		: pos(p), prevPos(p), vel(v), scale(sc), angle(a) {}

};


struct CScore : public Component
{
	int score{ 0 };

	CScore() = default;
	CScore(int s = 0) : score(s) {}
};


struct CState : public Component
{
	enum State {
		isGoingUp = 1,
		isFacingLeft = 1 << 1,
		isRunning = 1 << 2,
		isGoingDown = 1 << 3
	};
	unsigned int  state{ 0 };

	CState() = default;
	CState(unsigned int s) : state(s) {}
	bool test(unsigned int x) { return (state & x); }
	void set(unsigned int x) { state |= x; }
	void unSet(unsigned int x) { state &= ~x; }

};
#endif //SFMLCLASS_COMPONENTS_H
