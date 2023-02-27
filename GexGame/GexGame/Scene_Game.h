
#ifndef SFMLCLASS_SCENE_GAME_H
#define SFMLCLASS_SCENE_GAME_H

#include "Scene.h"

class Scene_Game : public Scene {

    struct PlayerConfig
    {
        float X{ 0.f }, Y{ 0.f };
        float SPEED{ 0.f }, MAXSPEED{ 0.f };
    };
	struct EnemyConfig
	{
		std::string name;
		float X{ 0.f }, Y{ 0.f };
		float targetX{ 0.f }, targetY{ 0.f };

	};

protected:

	std::shared_ptr<Entity>			m_player;
	std::string						m_levelPath;
	PlayerConfig					m_playerConfig;
	std::vector <EnemyConfig>		m_enemyConfigVec;
	bool							m_drawTextures{ true };
	bool							m_drawCollision{ false };
	bool							m_drawGrid{ false };
	const sf::Vector2f				m_gridSize{ 64,64 };
	sf::Text						m_gridText;
	bool							m_wonGame{false};
	mutable bool					playOnce{ true };

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
	void PlayerTilesCollisions(EntityVec& players, EntityVec& tiles, EntityVec& enemies);
	void GrassCollision(EntityVec& players);
	void PlayerEnemyCol(EntityVec& players, EntityVec& enemies);
	void checkEnemyTileCol(EntityVec& enemies, NttPtr& t);
	void EnemiesAI(const int& number, sf::Vector2f& ptx, NttPtr& e);
	void sDebug();
	void drawLine();

	void playerCheckState();


	sf::Vector2f gridToMidPixel(float gridX, float gridY, std::shared_ptr<Entity> entity);
	void loadLevel(const std::string& filename);
	void loadFromFile(const std::string& filename);
	void spawnPlayer();
	void spawnEnemies();
	void moveEnemies();
	void addPlayerPoints(int tiles);
	bool checkIfWon();
	void spawnBullet(std::shared_ptr<Entity>);

};


#endif //SFMLCLASS_SCENE_GAME_H
