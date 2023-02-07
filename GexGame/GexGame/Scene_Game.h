//
// Created by David Burchill on 2022-10-21.
//

#ifndef SFMLCLASS_SCENE_GAME_H
#define SFMLCLASS_SCENE_GAME_H

#include "Scene.h"

class Scene_Game : public Scene {

    struct PlayerConfig
    {
        float X{ 0.f }, Y{ 0.f };
        float SPEED{ 0.f }, MAXSPEED{ 0.f };
        std::string WEAPON;
    };
protected:

	std::shared_ptr<Entity>		m_player;
	std::string					m_levelPath;
	PlayerConfig				m_playerConfig;
	bool						m_drawTextures{ true };
	bool						m_drawCollision{ false };
	bool						m_drawGrid{ false };
	const sf::Vector2f			m_gridSize{ 64,64 };
	sf::Text					m_gridText;

	void	init(const std::string& levelPath);
	void	registerActions();
	void	onEnd() override;


public:
	Scene_Game(GameEngine* gameEngine, const std::string& levelPath);

	void update() override;
	void sRender() override;
	void sDoAction(const Action& action) override;

	void sMovement();
	void sAnimation();
	void sLifespan();
	void sEnemySpawner();
	void sCollision();

	void sDebug();
	void drawLine();

	void playerCheckState();


	sf::Vector2f gridToMidPixel(float gridX, float gridY, std::shared_ptr<Entity> entity);
	void loadLevel(const std::string& filename);
	void loadFromFile(const std::string& filename);
	void spawnPlayer();
	void spawnBullet(std::shared_ptr<Entity>);

};


#endif //SFMLCLASS_SCENE_GAME_H
