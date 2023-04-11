#include "Scene_Options.h"
#include "MusicPlayer.h"
#include "SoundPlayer.h"

void Scene_Options::init()
{
	registerAction(sf::Keyboard::W, "UP");
	registerAction(sf::Keyboard::Up, "UP");
	registerAction(sf::Keyboard::S, "DOWN");
	registerAction(sf::Keyboard::Down, "DOWN");
	registerAction(sf::Keyboard::Enter, "SELECT");
	registerAction(sf::Keyboard::Space, "SELECT");
	registerAction(sf::Keyboard::Escape, "QUIT");
	registerAction(sf::Keyboard::Q, "QUIT");

	m_title = "OPTIONS";

	m_menuItems.push_back(std::make_pair(true, "Music: " ));
	m_menuItems.push_back(std::make_pair(true, "Sound effects: " ));

	m_menuText.setFont(m_game->assets().getFont("MegaSurprise"));

	const size_t CHAR_SIZE{ 64 };
	m_menuText.setCharacterSize(CHAR_SIZE);
}

void Scene_Options::registerItem(SceneID key, std::string item)
{
	
}

void Scene_Options::onEnd()
{
	m_game->changeScene(SceneID::MENU, false);
}

Scene_Options::Scene_Options(GameEngine* gameEngine)
	:Scene(gameEngine)
{
	init();
}

void Scene_Options::update(sf::Time dt)
{
	m_entityManager.update();
}

void Scene_Options::sRender()
{
	sf::View view = m_game->window().getView();
	view.setCenter(m_game->window().getSize().x / 2.f, m_game->window().getSize().y / 2.f);
	m_game->window().setView(view);
	m_game->window().draw(m_background);

	m_background.setTexture(m_game->assets().getTexture("Levels"));
	m_background.setPosition(0.f, 0.f);

	static const sf::Color selectedColor(255, 255, 255);
	static const sf::Color normalColor(0, 0, 0);

	static const sf::Color backgroundColor(75, 148, 41);

	sf::Text footer("UP: W    DOWN: S   PLAY:D    QUIT: ESC",
		m_game->assets().getFont("MegaSurprise"), 20);
	footer.setFillColor(normalColor);
	footer.setPosition(32, 920);

	m_menuText.setFillColor(normalColor);
	m_menuText.setString(m_title);
	m_menuText.setPosition(40, 10);
	m_game->window().draw(m_menuText);

	for (size_t i{ 0 }; i < m_menuItems.size(); ++i)
	{
		m_menuText.setFillColor((i == m_menuIndex ? selectedColor : normalColor));
		m_menuText.setPosition(60, 32 + (i + 1) * 100);
		std::string state = m_menuItems.at(i).first == true ? "ON" : "OFF";
		m_menuText.setString(m_menuItems.at(i).second + state);
		m_game->window().draw(m_menuText);
	}

	m_game->window().draw(footer);
}

void Scene_Options::sDoAction(const Action& action)
{
	if (action.type() == "START")
	{
		if (action.name() == "UP")
		{
			m_menuIndex = (m_menuIndex + m_menuItems.size() - 1) % m_menuItems.size();
			playToggle();
		}
		else if (action.name() == "DOWN")
		{
			m_menuIndex = (m_menuIndex + 1) % m_menuItems.size();
			playToggle();
		}
		// TODO generalize
		else if (action.name() == "SELECT")
		{
			m_menuItems.at(m_menuIndex).first = !m_menuItems.at(m_menuIndex).first;
			if (m_menuIndex == 0 && m_menuItems.at(m_menuIndex).first == false)
				MusicPlayer::getInstance().setPaused(true);
			if (m_menuIndex == 0 && m_menuItems.at(m_menuIndex).first == true)
				MusicPlayer::getInstance().setPaused(false);

			if (m_menuIndex == 1 && m_menuItems.at(m_menuIndex).first == false)
				SoundPlayer::getInstance().setSoundState(false);
			if (m_menuIndex == 1 && m_menuItems.at(m_menuIndex).first == true)
				SoundPlayer::getInstance().setSoundState(true);		
			playToggle();
		}
		else if (action.name() == "QUIT")
		{
			onEnd();
		}
	}
}

void Scene_Options::playToggle()
{
	SoundPlayer::getInstance().play("toggle");
}
