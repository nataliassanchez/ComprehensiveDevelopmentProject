

#include <SFML/System/Time.hpp>
#include <fstream>
#include "Entity.h"
#include "Scene_Game.h"
#include "Action.h"
#include "GameEngine.h"
#include "Components.h"
#include "Physics.h"


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
}

void Scene_Game::registerActions()
{
	registerAction(sf::Keyboard::P, "PAUSE");
	registerAction(sf::Keyboard::Escape, "QUIT");

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
	m_entityManager.update();

	// TODO pause function

	sMovement();
	sLifespan();
	sCollision();
	sAnimation();

	playerCheckState();
}


void Scene_Game::sMovement()
{
	// player movement 
	auto& pt = m_player->getComponent<CTransform>();
	pt.vel.x = 0.f;
	pt.vel.y = 0.f;
	if (m_player->getComponent<CInput>().left)
		pt.vel.x -= 5;

	if (m_player->getComponent<CInput>().right)
		pt.vel.x += 5;

	if (m_player->getComponent<CInput>().up) {
		pt.vel.y -= 5;
	}
	if (m_player->getComponent<CInput>().down) {
		pt.vel.y += 5;
	}

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


	// move all entities
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

		if (std::abs(tx.vel.x) > 0.1f)
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

	for (auto p : players)
	{
		//p->getComponent<CState>().unSet(CState::isGrounded); // not grounded 
		for (auto t : tiles)
		{
			auto overlap = Physics::getOverlap(p, t);
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
						p->getComponent<CTransform>().pos.y -= overlap.y;
					}
					else
					{  // player hit something from below
						p->getComponent<CTransform>().pos.y += overlap.y;
					}
					p->getComponent<CTransform>().vel.y = 0.f;
				}


				// collision is in the x direction
				if (prevOverlap.y > 0)
				{
					if (ptx.prevPos.x < ttx.prevPos.x) // player left of tile
						p->getComponent<CTransform>().pos.x -= overlap.x;
					else
						p->getComponent<CTransform>().pos.x += overlap.x;
				}
			}
		}
	}
	auto tilesGrass = m_entityManager.getEntities("tileGrass");
	for (auto p : players) {
		for (auto t : tilesGrass)
		{
			auto overlap = Physics::getOverlap(p, t);
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
				if (prevOverlap.y > 0)
				{
					if (ptx.prevPos.x < ttx.prevPos.x) // player left of tile
						t->addComponent<CAnimation>(m_game->assets().getAnimation("Short"), true);
					else
						t->addComponent<CAnimation>(m_game->assets().getAnimation("Short"), true);
				}
			}
		}
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
	view.setCenter(centerX, m_game->window().getSize().y - view.getCenter().y);
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
		const int ROW0 = 1024;
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
			lines.append(sf::Vertex(sf::Vector2f(left, ROW0 - (lastRow - y) * m_gridSize.y)));
			lines.append(sf::Vertex(sf::Vector2f(right, ROW0 - (lastRow - y) * m_gridSize.y)));
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
		else if (action.name() == "LEFT") { m_player->getComponent<CInput>().left = true; }
		else if (action.name() == "RIGHT") { m_player->getComponent<CInput>().right = true; }
		else if (action.name() == "UP") { m_player->getComponent<CInput>().up = true; }
		else if (action.name() == "DOWN") { m_player->getComponent<CInput>().down = true; }

	}


	// on Key Release 
	else if (action.type() == "END")
	{
		if (action.name() == "LEFT") { m_player->getComponent<CInput>().left = false; }
		else if (action.name() == "RIGHT") { m_player->getComponent<CInput>().right = false; }
		else if (action.name() == "UP") { m_player->getComponent<CInput>().up = false; }
		else if (action.name() == "DOWN") { m_player->getComponent<CInput>().down = false; }
	
	}
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
	m_game->changeScene(SceneID::MENU, true);
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
	float y = 1024.f - gridY * m_gridSize.y;

	sf::Vector2f spriteSize = entity->getComponent<CAnimation>().animation.getSize();

	return sf::Vector2f(x + spriteSize.x / 2.f, y - spriteSize.y / 2.f);
}

void Scene_Game::loadLevel(const std::string& path)
{

	m_entityManager = EntityManager(); // get a new entity manager

	// TODO read in level file 
	loadFromFile(path);

	spawnPlayer();

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
				m_playerConfig.MAXSPEED >>
				m_playerConfig.WEAPON;
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
	m_player->addComponent<CBoundingBox>(sf::Vector2f(64, 64));
	m_player->addComponent<CState>();

}

void Scene_Game::spawnBullet(std::shared_ptr<Entity> e)
{
}