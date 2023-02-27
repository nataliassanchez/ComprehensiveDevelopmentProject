

#include <SFML/System/Time.hpp>
#include <fstream>
#include "Entity.h"
#include "Scene_Game.h"
#include "Action.h"
#include "GameEngine.h"
#include "Components.h"
#include "Physics.h"
#include "MusicPlayer.h"
#include "SoundPlayer.h"
#include <random>

namespace {
	std::random_device rd;
	std::mt19937 rng(rd());
}


Scene_Game::Scene_Game(GameEngine *gameEngine, const std::string &levelPath)
        : Scene(gameEngine), m_levelPath(levelPath) {

    init(m_levelPath);
}


void Scene_Game::init(const std::string& levelPath)
{
	registerActions();


	m_gridText.setCharacterSize(12);
	m_gridText.setFont(m_game->assets().getFont("Arial"));

	loadLevel(levelPath);

	MusicPlayer::getInstance().stop();
	//MusicPlayer::getInstance().play("menuTheme");
	//MusicPlayer::getInstance().setVolume(15);

}

void Scene_Game::registerActions()
{
	registerAction(sf::Keyboard::P, "PAUSE");
	registerAction(sf::Keyboard::Escape, "QUIT");
	registerAction(sf::Keyboard::Q, "QUIT");

	registerAction(sf::Keyboard::T, "TOGGLE_TEXTURE");
	registerAction(sf::Keyboard::C, "TOGGLE_COLLISION");
	registerAction(sf::Keyboard::G, "TOGGLE_GRID");

	registerAction(sf::Keyboard::A, "LEFT");
	registerAction(sf::Keyboard::Left, "LEFT");
	registerAction(sf::Keyboard::D, "RIGHT");
	registerAction(sf::Keyboard::Right, "RIGHT");

	registerAction(sf::Keyboard::W, "UP");
	registerAction(sf::Keyboard::Up, "UP");

	registerAction(sf::Keyboard::S, "DOWN");
	registerAction(sf::Keyboard::Down, "DOWN");
}


void Scene_Game::update()
{
	if (m_isPaused)
		return;

	m_entityManager.update();

	if (checkIfWon() || m_isGameOver)
		return;

	// TODO pause function
	

	sMovement();
	sLifespan();
	sCollision();
	sAnimation();

	playerCheckState();
	
	SoundPlayer::getInstance().removeStoppedSounds();
	
}


void Scene_Game::sMovement()
{
	// player movement 
	auto& pt = m_player->getComponent<CTransform>();
	pt.vel.x = 0.f;
	pt.vel.y = 0.f;
	if (m_player->getComponent<CInput>().left)
		pt.vel.x = -5;

	if (m_player->getComponent<CInput>().right)
		pt.vel.x = 5;

	if (m_player->getComponent<CInput>().up)
		pt.vel.y = -5;
	
	if (m_player->getComponent<CInput>().down) 
		pt.vel.y = 5;
	

	// facing direction
	if (pt.vel.x < -0.1)
		m_player->getComponent<CState>().set(CState::isFacingLeft);
	if (pt.vel.x > 0.1)
		m_player->getComponent<CState>().unSet(CState::isFacingLeft);

	if (pt.vel.y > 0.1) {
		m_player->getComponent<CState>().unSet(CState::isGoingUp);
		m_player->getComponent<CState>().set(CState::isGoingDown);		
	}

	if (pt.vel.y < -0.1) {
		m_player->getComponent<CState>().unSet(CState::isGoingDown);
		m_player->getComponent<CState>().set(CState::isGoingUp);
	}

	 //move enemies


	for (auto e : m_entityManager.getEntities())
	{
		auto& tx = e->getComponent<CTransform>();
		tx.prevPos = tx.pos;
		tx.pos += tx.vel;
	}
}

void Scene_Game::playerCheckState()
{
	auto& tx = m_player->getComponent<CTransform>();
	auto& state = m_player->getComponent<CState>();

	// face the right way
	if (std::abs(tx.vel.x) > 0.1f)
		tx.scale.x = (tx.vel.x > 0.f) ? 1.f : -1.f;

		if (std::abs(tx.vel.x) != 0.f)
		{
			if (!state.test(CState::isRunning)) // wasn't running
			{
				// change to running animation
				m_player->addComponent<CAnimation>(m_game->assets().getAnimation("Run"), true);
				state.set(CState::isRunning);
			}
			
		}
		else if (std::abs(tx.vel.y) > 0.f) {

			if (state.test(CState::isGoingUp)) {
				m_player->addComponent<CAnimation>(m_game->assets().getAnimation("Back"), true);
				state.set(CState::isGoingUp);
			}
			else {
				m_player->addComponent<CAnimation>(m_game->assets().getAnimation("Front"), true);
				state.set(CState::isGoingDown);
			}
		}
		else
		{
			m_player->addComponent<CAnimation>(m_game->assets().getAnimation("Stand"), true);
			state.unSet(CState::isRunning);
		}
	
}

void Scene_Game::sLifespan()
{

	

}

void Scene_Game::sEnemySpawner()
{
}

void Scene_Game::sCollision()

{
	auto players = m_entityManager.getEntities("player");
	auto tiles = m_entityManager.getEntities("tile");
	auto enemies = m_entityManager.getEntities("enemy");

	//Check player and tiles collisions
	PlayerTilesCollisions(players, tiles, enemies);

	//Check enemies and player collisions
	PlayerEnemyCol(players, enemies);

	//Check lawn mowner and player collision
	GrassCollision(players);
}

void Scene_Game::PlayerTilesCollisions(EntityVec& players, EntityVec& tiles, EntityVec& enemies)
{
	for (auto p : players)
	{
		for (auto t : tiles)
		{
			auto overlap = Physics::getOverlap(p, t);
			if (overlap.x > 0 && overlap.y > 0)
			{
				auto prevOverlap = Physics::getPreviousOverlap(p, t);
				auto& ptx = p->getComponent<CTransform>();
				auto ttx = t->getComponent<CTransform>();


				// collision is in the y direction
				if (prevOverlap.x > 0)
				{
					if (ptx.prevPos.y < ttx.prevPos.y)
						p->getComponent<CTransform>().pos.y -= overlap.y;
					else
						p->getComponent<CTransform>().pos.y += overlap.y;
					
					p->getComponent<CTransform>().vel.y = 0.f;

				}

				// collision is in the x direction
				if (prevOverlap.y > 0)
				{
					if (ptx.prevPos.x < ttx.prevPos.x) 
						p->getComponent<CTransform>().pos.x -= overlap.x;
					else
						p->getComponent<CTransform>().pos.x += overlap.x;
				}

			}
			//Check enemies and tiles collisions
			checkEnemyTileCol(enemies, t);
		}
	}
}

void Scene_Game::GrassCollision(EntityVec& players)
{
	auto tilesGrass = m_entityManager.getEntities("tileGrass");
	for (auto p : players) {
		for (auto t : tilesGrass)
		{
			auto overlap = Physics::getOverlap(p, t);
			int tilesInARow = 0;


			if (overlap.x > 0 && overlap.y > 0)  // +ve overlap in both x and y means collision
			{
				auto prevOverlap = Physics::getPreviousOverlap(p, t);
				auto& ptx = p->getComponent<CTransform>();
				auto ttx = t->getComponent<CTransform>();


				// collision is in the y direction
				if (prevOverlap.x > 0)
				{
					if (ptx.prevPos.y < ttx.prevPos.y)
					{   // player standing on something isGrounded
						t->addComponent<CAnimation>(m_game->assets().getAnimation("Short"), true);
					}
					else
					{  // player hit something from below
						t->addComponent<CAnimation>(m_game->assets().getAnimation("Short"), true);
					}
				}
				// collision is in the x direction
				addPlayerPoints(tilesInARow);

			}
		}
	}
}

void Scene_Game::PlayerEnemyCol(EntityVec& players, EntityVec& enemies)
{
	for (auto p : players) {
		for (auto e : enemies) {
			auto overlap = Physics::getOverlap(p, e);
			if (overlap.x > 0 && overlap.y > 0)  // +ve overlap in both x and y means collision
			{
				auto prevOverlap = Physics::getPreviousOverlap(p, e);
				auto& ptx = p->getComponent<CTransform>();
				auto ttx = e->getComponent<CTransform>();


				// collision is in the y direction
				if (prevOverlap.x > 0) m_isGameOver = true;

				// collision is in the x direction
				if (prevOverlap.y > 0) m_isGameOver = true;
				
			}

		}
	}
}

void Scene_Game::checkEnemyTileCol(EntityVec& enemies, NttPtr& t)
{
	std::uniform_int_distribution rand(0, 3);
	auto number = rand(rng);
	for (auto e : enemies)
	{
		auto overlap = Physics::getOverlap(e, t);
		if (overlap.x > 0 && overlap.y > 0)  // +ve overlap in both x and y means collision
		{
			auto prevOverlap = Physics::getPreviousOverlap(e, t);
			auto& ptx = e->getComponent<CTransform>();
			auto ttx = t->getComponent<CTransform>();


			// collision is in the y direction
			if (prevOverlap.x > 0)
			{
				if (ptx.prevPos.y < ttx.prevPos.y)
				{   // player standing on something isGrounded
					e->getComponent<CTransform>().pos.y -= overlap.y;
				}
				else
				{  // player hit something from below
					e->getComponent<CTransform>().pos.y += overlap.y;
				}

			}

			// collision is in the x direction
			if (prevOverlap.y > 0)
			{
				if (ptx.prevPos.x < ttx.prevPos.x) // player left of tile
					e->getComponent<CTransform>().pos.x -= overlap.x;
				else
					e->getComponent<CTransform>().pos.x += overlap.x;
			}

			EnemiesAI(number, ptx.vel, e);

		}
	}
}

void Scene_Game::EnemiesAI(const int& number, sf::Vector2f& ptx, NttPtr& e)
{
	switch (number)
	{
	case 0:
		ptx.x = 2.f;
		ptx.y = 0.f;
		ptx.x *= -1;
		break;
	case 1:
		ptx.x = 0.f;
		ptx.y = 2.f;
		ptx.y *= -1;
		break;
	case 2:
		ptx.x = 0.f;
		ptx.y = -2.f;
		ptx.y *= -1;
		break;
	case 3:
		ptx.x = -2.f;
		ptx.y = 0.f;
		ptx.x *= -1;
		break;
	default:
		break;
	}
	if ((e->getComponent<CAnimation>().animation.getName() == "PurcoRun" ||
		e->getComponent<CAnimation>().animation.getName() == "PurcoD" ||
		e->getComponent<CAnimation>().animation.getName() == "PurcoB")
		&& ptx.y < 0) {
		e->getComponent<CAnimation>().animation = m_game->assets().getAnimation("PurcoUp");
	}
	if ((e->getComponent<CAnimation>().animation.getName() == "PurcoRun" ||
		e->getComponent<CAnimation>().animation.getName() == "PurcoUp" ||
		e->getComponent<CAnimation>().animation.getName() == "PurcoB") &&
		ptx.y > 0) {
		e->getComponent<CAnimation>().animation = m_game->assets().getAnimation("PurcoD");
	}

	if ((e->getComponent<CAnimation>().animation.getName() == "PurcoD" ||
		e->getComponent<CAnimation>().animation.getName() == "PurcoUp" ||
		e->getComponent<CAnimation>().animation.getName() == "PurcoB")
		&& ptx.y == 0 && ptx.x < 0) {
		e->getComponent<CAnimation>().animation = m_game->assets().getAnimation("PurcoRun");
	}
	if ((e->getComponent<CAnimation>().animation.getName() == "PurcoD" ||
		e->getComponent<CAnimation>().animation.getName() == "PurcoUp" ||
		e->getComponent<CAnimation>().animation.getName() == "PurcoR")
		&& ptx.y == 0 && ptx.x > 0) {
		e->getComponent<CAnimation>().animation = m_game->assets().getAnimation("PurcoB");
	}
}

void Scene_Game::sRender()
{
	// background changes if paused
	static const sf::Color bkg(100, 100, 255);
	static const sf::Color pauseBackground(50, 50, 150);
	m_game->window().clear((m_isPaused ? pauseBackground : bkg));

	// set the view to center on the player 
	// this is a side scroller so only worry about X axis
	auto& pPos = m_player->getComponent<CTransform>().pos;
	float centerX = std::max(m_game->window().getSize().x / 2.f, pPos.x);

	sf::View view = m_game->window().getView();
	view.setCenter(pPos);

	m_game->window().setView(view);


	// draw all entities
	if (m_drawTextures)
	{
		for (auto e : m_entityManager.getEntities())
		{
	
			if (e->hasComponent<CAnimation>())
			{
				auto& transform = e->getComponent<CTransform>();
				auto& animation = e->getComponent<CAnimation>().animation;

				animation.getSprite().setRotation(transform.angle);
				animation.getSprite().setPosition(transform.pos.x, transform.pos.y);
				animation.getSprite().setScale(transform.scale.x, transform.scale.y);
				m_game->window().draw(animation.getSprite());

				static sf::Text text("Score: ", m_game->assets().getFont("Arial"), 15);
				if (e->hasComponent<CScore>()) {
					int score = e->getComponent<CScore>().score;
					std::string str = "Score: " + std::to_string(score);
					text.setString(str);
					centerOrigin(text);

					sf::Vector2f offset(0.f, 40.f);
					text.setPosition(transform.pos + offset);
					m_game->window().draw(text);
				}
			}
			
		}
		for (auto e : m_entityManager.getEntities("enemy"))
		{

			if (e->hasComponent<CAnimation>())
			{
				auto& transform = e->getComponent<CTransform>();
				auto& animation = e->getComponent<CAnimation>().animation;

				animation.getSprite().setRotation(transform.angle);
				animation.getSprite().setPosition(transform.pos.x, transform.pos.y);
				animation.getSprite().setScale(transform.scale.x, transform.scale.y);
				m_game->window().draw(animation.getSprite());
			}
		}

	}

	

	if (m_drawCollision)
	{
		for (auto e : m_entityManager.getEntities())
		{
			if (e->hasComponent<CBoundingBox>())
			{
				auto& box = e->getComponent<CBoundingBox>();
				auto& transform = e->getComponent<CTransform>();
				sf::RectangleShape rect;
				rect.setSize(sf::Vector2f(box.size.x, box.size.y));
				rect.setOrigin(sf::Vector2f(box.halfSize.x, box.halfSize.y));
				rect.setPosition(transform.pos.x, transform.pos.y);
				rect.setFillColor(sf::Color(0, 0, 0, 0));
				rect.setOutlineColor(sf::Color(0, 255, 0));
				rect.setOutlineThickness(1.f);
				m_game->window().draw(rect);
			}
		}
	}

	if (m_isGameOver) {
		sf::Text gameOver("GAME OVER", m_game->assets().getFont("Mario"), 200);
		sf::Text returnText("Q: Return", m_game->assets().getFont("Mario"), 40);

		SoundPlayer::getInstance().removeStoppedSounds();

		if (checkIfWon()) {
			gameOver.setString("YOU WON");
			if (playOnce) {
				SoundPlayer::getInstance().play("victory", view.getCenter());
				playOnce = false;
			}
		}
		
		centerOrigin(gameOver);
		centerOrigin(returnText);

		sf::View view = m_game->window().getView();
		view.setCenter(m_game->window().getSize().x / 2.f, m_game->window().getSize().y / 2.f);
		m_game->window().setView(view);

		gameOver.setPosition(view.getCenter());
		returnText.setPosition(view.getCenter().x, view.getCenter().y + gameOver.getLocalBounds().height);
		m_game->window().draw(gameOver);
		m_game->window().draw(returnText);

		if (playOnce) {
			SoundPlayer::getInstance().play("defeat", view.getCenter());
			playOnce = false;
		}
		//MusicPlayer::getInstance().stop();
	}

	if (m_isPaused) {
		sf::Text paused("PAUSED", m_game->assets().getFont("Mario"), 200);
		sf::Text returnText("Q: RETURN  P: CONTINUE", m_game->assets().getFont("Mario"),40);

		centerOrigin(paused);
		centerOrigin(returnText);

		paused.setPosition(view.getCenter().x, view.getCenter().y);
		returnText.setPosition(view.getCenter().x, view.getCenter().y + paused.getLocalBounds().height);

		m_game->window().draw(paused);
		m_game->window().draw(returnText);

		MusicPlayer::getInstance().setPaused(!m_isPaused);
	}


	//// draw grid
	sf::VertexArray lines(sf::Lines);
	sf::Text gridText;
	gridText.setFont(m_game->assets().getFont("Arial"));
	gridText.setCharacterSize(10);
	
	if (m_drawGrid)
	{
		float left = view.getCenter().x - view.getSize().x / 2.f;
		float right = left + view.getSize().x;
		float top = view.getCenter().y - view.getSize().y / 2.f;
		float bot = top + view.getSize().y;
	
		// aling grid to grid size
		int nCols = static_cast<int>(view.getSize().x) / m_gridSize.x;
		int nRows = static_cast<int>(view.getSize().y) / m_gridSize.y;
	
	
		// row and col # of bot left
		const int ROW0 = 2048;
		int firstCol = static_cast<int>(left) / static_cast<int>(m_gridSize.x);
		int lastRow = static_cast<int>(bot) / static_cast<int>(m_gridSize.y);
	
		lines.clear();
	
		// vertical lines
	
		for (int x{ 0 }; x <= nCols; ++x)
		{
			lines.append(sf::Vector2f((firstCol + x) * m_gridSize.x, top));
			lines.append(sf::Vector2f((firstCol + x) * m_gridSize.x, bot));
		}
	
	
		// horizontal lines
		for (int y{ 0 }; y <= nRows; ++y)
		{
			lines.append(sf::Vertex(sf::Vector2f(left,  (lastRow - y) * m_gridSize.y)));
			lines.append(sf::Vertex(sf::Vector2f(right, (lastRow - y) * m_gridSize.y)));
		}
	
	
		// grid coordinates
		// (firstCol, lastRow) is the bottom left corner of the view 
		for (int x{ 0 }; x <= nCols; ++x)
		{
			for (int y{ 0 }; y <= nRows; ++y)
			{
				std::string label = std::string("(" + std::to_string(firstCol + x) + ", " + std::to_string(lastRow - y) + ")");
				gridText.setString(label);
				gridText.setPosition((x + firstCol) * m_gridSize.x, ROW0 - (lastRow - y + 1) * m_gridSize.y);
				m_game->window().draw(gridText);
			}
		}
	
	
		m_game->window().draw(lines);
	}

	//m_game->window().display();
}

void Scene_Game::sDoAction(const Action& action)
{

	// On Key Press
	if (action.type() == "START")
	{
		if (action.name() == "TOGGLE_TEXTURE") { m_drawTextures = !m_drawTextures; }
		else if (action.name() == "TOGGLE_COLLISION") { m_drawCollision = !m_drawCollision; }
		else if (action.name() == "TOGGLE_GRID") { m_drawGrid = !m_drawGrid; }
		else if (action.name() == "PAUSE") { setPaused(!m_isPaused); }
		else if (action.name() == "QUIT") { onEnd(); }


		// Player control
		else if (action.name() == "LEFT") { 
			m_player->getComponent<CInput>().left = true; 
			m_player->getComponent<CInput>().right = false;
			m_player->getComponent<CInput>().up = false;
			m_player->getComponent<CInput>().down = false;
		}
		else if (action.name() == "RIGHT") { 
			m_player->getComponent<CInput>().right = true; 
			m_player->getComponent<CInput>().left = false;
			m_player->getComponent<CInput>().up = false;
			m_player->getComponent<CInput>().down = false;
		}
		else if (action.name() == "UP") { 
			m_player->getComponent<CInput>().up = true; 
			m_player->getComponent<CInput>().down = false;
			m_player->getComponent<CInput>().left = false;
			m_player->getComponent<CInput>().right = false;
		}
		else if (action.name() == "DOWN") { 
			m_player->getComponent<CInput>().down = true;
			m_player->getComponent<CInput>().up = false;
			m_player->getComponent<CInput>().left = false;
			m_player->getComponent<CInput>().right = false;
		}

	}


	// on Key Release 
	//else if (action.type() == "END")
	//{
	//	if (action.name() == "LEFT") { m_player->getComponent<CInput>().left = false; }
	//	else if (action.name() == "RIGHT") { m_player->getComponent<CInput>().right = false; }
	//	else if (action.name() == "UP") { m_player->getComponent<CInput>().up = false; }
	//	else if (action.name() == "DOWN") { m_player->getComponent<CInput>().down = false; }
	//
	//}
}


void Scene_Game::sAnimation()
{

	// m_player->getComponent<CAnimation>().animation.update();

	for (auto e : m_entityManager.getEntities())
	{
		auto& anim = e->getComponent<CAnimation>();
		if (anim.has)
		{
			anim.animation.update(anim.repeat);
			if (anim.animation.hasEnded())
				e->destroy();
		}
	}
}


void Scene_Game::onEnd()
{
	m_game->changeScene(SceneID::LEVELS, true);
}

void Scene_Game::drawLine()
{
}

void Scene_Game::sDebug()
{
}

sf::Vector2f Scene_Game::gridToMidPixel(float gridX, float gridY, std::shared_ptr<Entity> entity)
{
	// (left, bot) of grix,gidy)

	// this is for side scroll, and based on window height being the same as world height
	// to be more generic and support scrolling up and down as well as left and right it
	// should be based on world size not window size
	float x = 0.f + gridX * m_gridSize.x;
	float y = 2048.f - gridY * m_gridSize.y;

	sf::Vector2f spriteSize = entity->getComponent<CAnimation>().animation.getSize();

	return sf::Vector2f(x + spriteSize.x / 2.f, y - spriteSize.y / 2.f);
}

void Scene_Game::loadLevel(const std::string& path)
{

	m_entityManager = EntityManager(); // get a new entity manager

	// TODO read in level file 
	loadFromFile(path);

	spawnPlayer();
	spawnEnemies();


}

void Scene_Game::loadFromFile(const std::string& path)
{
	// Read Config file 
	std::ifstream confFile(path);
	if (confFile.fail())
	{
		std::cerr << "Open file: " << path << " failed\n";
		confFile.close();
		exit(1);
	}

	std::string token{ "" };
	confFile >> token;
	while (confFile)
	{
		if (token == "Tile")
		{
			std::string name;
			float gx, gy;
			confFile >> name >> gx >> gy;

			auto e = m_entityManager.addEntity("tile");
			e->addComponent<CAnimation>(m_game->assets().getAnimation(name), true);
			e->addComponent<CBoundingBox>(m_game->assets().getAnimation(name).getSize());
			e->addComponent<CTransform>(gridToMidPixel(gx, gy, e));
		}
		else if (token == "TileGrass")
		{
			std::string name;
			float gx, gy;
			confFile >> name >> gx >> gy;

			auto e = m_entityManager.addEntity("tileGrass");
			e->addComponent<CAnimation>(m_game->assets().getAnimation(name), true);
			e->addComponent<CBoundingBox>(m_game->assets().getAnimation(name).getSize());
			e->addComponent<CTransform>(gridToMidPixel(gx, gy, e));
		}
		else if (token == "Dec")
		{
			std::string name;
			float gx, gy;
			confFile >> name >> gx >> gy;

			auto e = m_entityManager.addEntity("dec");
			e->addComponent<CAnimation>(m_game->assets().getAnimation(name), true);
			e->addComponent<CTransform>(gridToMidPixel(gx, gy, e));
		}
		else if (token == "Player")
		{

			confFile >>
				m_playerConfig.X >>
				m_playerConfig.Y >>
				m_playerConfig.SPEED >>
				m_playerConfig.MAXSPEED;
		}

		else if (token == "Enemy")
		{
			EnemyConfig enemy;

			confFile >>
				enemy.name >>
				enemy.X >>
				enemy.Y >>
				enemy.targetX >>
				enemy.targetY;

			m_enemyConfigVec.push_back(enemy);
		}

		else if (token == "#")
		{
			; // ignore comments
			std::string tmp;
			std::getline(confFile, tmp);
			std::cout << "# " << tmp << "\n";
		}
		else
		{
			std::cerr << "Unkown asset type: " << token << std::endl;
		}

		confFile >> token;
	}
}


void Scene_Game::spawnPlayer()
{

	m_player = m_entityManager.addEntity("player");
	m_player->addComponent<CAnimation>(m_game->assets().getAnimation("Run"), true);
	m_player->addComponent<CTransform>(gridToMidPixel(m_playerConfig.X, m_playerConfig.Y, m_player));
	m_player->addComponent<CBoundingBox>(sf::Vector2f(50, 50));
	m_player->addComponent<CState>();
	m_player->addComponent<CScore>(0);

}

void Scene_Game::spawnEnemies()
{
	for (EnemyConfig e : m_enemyConfigVec) {
		auto enemy = m_entityManager.addEntity("enemy");
		
		
		if (e.name == "purcopine") {
			enemy->addComponent<CAnimation>(m_game->assets().getAnimation("PurcoRun"), true);
			//enemy->addComponent<CBoundingBox>(m_game->assets().getAnimation("PurcoRun").getSize());
			enemy->addComponent<CBoundingBox>(sf::Vector2f(64,64));
		}
		if (e.name == "dog") {
			enemy->addComponent<CAnimation>(m_game->assets().getAnimation(""), true);
		}
		enemy->addComponent<CState>();
		auto & etx = enemy->addComponent<CTransform>(gridToMidPixel(e.X, e.Y, enemy));
		etx.vel.y = 0.f;
		etx.vel.x = 0.f;
		enemy->addComponent<CDirection>();

	}
}

void Scene_Game::moveEnemies() 
{
	
}

void Scene_Game::addPlayerPoints(int tiles)
{
	int cutTiles = 0;
	for (auto grass : m_entityManager.getEntities("tileGrass")) {
		if (grass->getComponent<CAnimation>().animation.getName() == "Short")
			cutTiles += 1;			
	}
	if (cutTiles > 10) 
		cutTiles += 10;
	

	m_player->getComponent<CScore>().score = cutTiles;

}

bool Scene_Game::checkIfWon()
{
	auto tilesGrass = m_entityManager.getEntities("tileGrass");
	int counter = 0;
	for (auto tg : tilesGrass) {

		auto grass = tg->getComponent<CAnimation>().animation;
		if (grass.getName() == m_game->assets().getAnimation("Short").getName()) 
			counter++;	

	}
	if (counter == tilesGrass.size()) {
		m_wonGame = true;
		m_isGameOver = true;
	}

	return m_wonGame;
}

void Scene_Game::spawnBullet(std::shared_ptr<Entity> e)
{
}