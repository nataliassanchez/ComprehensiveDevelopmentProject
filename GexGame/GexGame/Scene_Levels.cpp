#include "Scene_Levels.h"

void Scene_Levels::init()
{
	registerAction(sf::Keyboard::W, "UP");
	registerAction(sf::Keyboard::Up, "UP");
	registerAction(sf::Keyboard::S, "DOWN");
	registerAction(sf::Keyboard::Down, "DOWN");
	registerAction(sf::Keyboard::Enter, "PLAY");
	registerAction(sf::Keyboard::Escape, "QUIT");
	registerAction(sf::Keyboard::Q, "QUIT");

	m_title = "LEVELS";
	registerItem(SceneID::LEVEL1,"Level 1");
	registerItem(SceneID::LEVEL2, "Level 2");
	registerItem(SceneID::LEVEL3, "Level 3");
	registerItem(SceneID::LEVEL4, "Level 4");

	m_levelPaths.push_back("../assets/level1.txt");
	m_levelPaths.push_back("../assets/level2.txt");
	m_levelPaths.push_back("../assets/level3.txt");
	m_levelPaths.push_back("../assets/level4.txt");

	m_menuText.setFont(m_game->assets().getFont("Mario"));

	const size_t CHAR_SIZE{ 64 };
	m_menuText.setCharacterSize(CHAR_SIZE);
}

void Scene_Levels::registerItem(SceneID key, std::string item) {
	m_menuItems.push_back(std::make_pair(key, item));
}

void Scene_Levels::onEnd()
{
	m_game->changeScene(SceneID::MENU, true);
}

Scene_Levels::Scene_Levels(GameEngine* gameEngine)
	:Scene(gameEngine)
{
	init();
}

void Scene_Levels::update()
{
	m_entityManager.update();
}

void Scene_Levels::sRender()
{
	sf::View view = m_game->window().getView();
	view.setCenter(m_game->window().getSize().x / 2.f, m_game->window().getSize().y / 2.f);
	m_game->window().setView(view);

	static const sf::Color selectedColor(255, 255, 255);
	static const sf::Color normalColor(0, 0, 0);

	static const sf::Color backgroundColor(100, 100, 255);

	sf::Text footer("UP: W    DOWN: S   PLAY:D    QUIT: ESC",
		m_game->assets().getFont("MegaSurprise"), 20);
	footer.setFillColor(normalColor);
	footer.setPosition(32, 700);

	m_game->window().clear(backgroundColor);

	m_menuText.setFillColor(normalColor);
	m_menuText.setString(m_title);
	m_menuText.setPosition(10, 10);
	m_game->window().draw(m_menuText);

	for (size_t i{ 0 }; i < m_menuItems.size(); ++i)
	{
		m_menuText.setFillColor((i == m_menuIndex ? selectedColor : normalColor));
		m_menuText.setPosition(32, 32 + (i + 1) * 96);
		m_menuText.setString(m_menuItems.at(i).second);
		m_game->window().draw(m_menuText);
	}

	m_game->window().draw(footer);
}

void Scene_Levels::sDoAction(const Action& action)
{
	if (action.type() == "START")
	{
		if (action.name() == "UP")
		{
			m_menuIndex = (m_menuIndex + m_menuItems.size() - 1) % m_menuItems.size();
		}
		else if (action.name() == "DOWN")
		{
			m_menuIndex = (m_menuIndex + 1) % m_menuItems.size();
		}
		// TODO generalize
		else if (action.name() == "PLAY")
		{
			m_game->changeScene(m_menuItems.at(m_menuIndex).first);
		}
		else if (action.name() == "QUIT")
		{
			onEnd();
		}
	}
}
