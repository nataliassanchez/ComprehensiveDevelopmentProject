//
// Created by David Burchill on 2022-11-24.
//

#ifndef SFMLCLASS_ANIMATION_H
#define SFMLCLASS_ANIMATION_H

#include <SFML/Graphics.hpp>
#include <vector>

class Animation {
public:
    std::string                 m_name{"none"};
    std::vector<sf::IntRect>    m_frames;
    size_t                      m_currentFrame{0};
    sf::Time                    m_timePerFrame;
    sf::Time                    m_countDown{sf::Time::Zero};
    bool                        m_isRepeating{true};
    bool                        m_hasEnded{false};
    sf::Sprite                  m_sprite;


public:
    Animation() = default;
    Animation(const std::string& name, const sf::Texture& t,
              std::vector<sf::IntRect> frames, sf::Time tpf, bool repeats=true);

    void update(sf::Time dt);
    bool hasEnded() const;
    const std::string& getName() const;
    sf::Sprite& getSprite();
};


#endif //SFMLCLASS_ANIMATION_H
