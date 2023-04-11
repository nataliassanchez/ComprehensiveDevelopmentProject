
#ifndef SFMLCLASS_SCENE_GAME_H
#define SFMLCLASS_SCENE_GAME_H

#include "Scene.h"

static long int globalScore{ 0 };
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
	};

protected:

	sf::Vector2u            m_windowSize{ 1728, 704 };
	//sf::RenderWindow        m_window;

	std::shared_ptr<Entity>			m_player;
	std::string						m_levelPath;
	PlayerConfig					m_playerConfig;
	std::vector <EnemyConfig>		m_enemyConfigVec;
	std::vector <std::pair<int, int>>m_coordinatesConfig;
	bool							m_drawTextures{ true };
	bool							m_drawCollision{ false };
	bool							m_drawGrid{ false };
	const sf::Vector2f				m_gridSize{ 64,64 };
	sf::Text						m_gridText;
	bool							m_wonGame{false};
	mutable bool					playOnce{ true };
	mutable bool					drawOnce{ true };
	mutable int						bonus{ 0 };
	mutable int						negativePoints{ 0 };
	sf::Text						m_statisticsScore;
	sf::Sprite						m_background;

	void	init(const std::string& levelPath);
	void	registerActions();
	void	onEnd() override;


public:
	Scene_Game(GameEngine* gameEngine, const std::string& levelPath);

	void update(sf::Time dt) override;
	void sRender() override;
	void renderEntity(std::string entity);
	void sDoAction(const Action& action) override;

	void sMovement();
	void sAnimation();
	void sLifespan(sf::Time dt);
	void sCollision();
	void dogObstables(EntityVec& gt, EntityVec& e);
	void playerTilesCollisions(EntityVec& players, EntityVec& tiles, EntityVec& enemies);
	void grassCollision(EntityVec& players);
	void playerEnemyCol(EntityVec& players, EntityVec& enemies);
	void playerObstablesCol();
	void checkEnemyTileCol(EntityVec& enemies, NttPtr& t);
	void enemiesAI(const int& number, sf::Vector2f& ptx, NttPtr& e);

	void playerCheckState();


	sf::Vector2f gridToMidPixel(float gridX, float gridY, std::shared_ptr<Entity> entity);
	void loadLevel(const std::string& filename);
	void loadFromFile(const std::string& filename);
	void spawnPlayer();
	void spawnEnemies();
	void spawnBonus();
	void addPlayerPoints();
	bool checkIfWon();
	void spawnPoop(sf::Vector2f pos);
	void bonusCollisions();

};


#endif //SFMLCLASS_SCENE_GAME_H
