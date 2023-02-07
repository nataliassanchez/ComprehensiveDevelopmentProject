//
// Created by David Burchill on 2022-11-24.
//

#ifndef SFMLCLASS_ANIMATION_H
#define SFMLCLASS_ANIMATION_H

#include <SFML/Graphics.hpp>
#include <vector>

class Animation {
private:
	sf::Sprite	m_sprite;
	size_t		m_frameCount{ 1 };		// number of frames in animation
	size_t		m_currentFrame{ 0 };	// the current from being played
	size_t		m_speed{ 0 };			// how many game frames in one animation frame
	sf::Vector2f	m_size{ 1,1 };			// width, height of one frame 
	std::string m_name{ "none" };
	bool		m_hasEnded{ false };

public:

	Animation();
	Animation(const std::string& name, const sf::Texture& t);
	Animation(const std::string& name, const sf::Texture& t, size_t frameCount, size_t speed);

	void				update(bool repeat = true);
	bool				hasEnded() const;
	const std::string& getName() const;
	const sf::Vector2f& getSize() const;
	sf::Sprite& getSprite();
};


#endif //SFMLCLASS_ANIMATION_H
