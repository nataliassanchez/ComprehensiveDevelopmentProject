#include <SFML/Window/Keyboard.hpp>
#include "Scene_Menu.h"
#include "GameEngine.h"
#include "Scene_Game.h"
#include <memory>
#include "Utilities.h"
#include "Components.h"
#include "SoundPlayer.h"



Scene_Menu::Scene_Menu(GameEngine* gameEngine)
        : Scene(gameEngine)
{
    init();
}

void Scene_Menu::init()
{
    registerAction(sf::Keyboard::D,			"LEFT");
    registerAction(sf::Keyboard::Left,		"LEFT");

    registerAction(sf::Keyboard::A,			"RIGHT");
    registerAction(sf::Keyboard::Right,	 	"RIGHT");

    registerAction(sf::Keyboard::Enter,			"PLAY");

    registerAction(sf::Keyboard::Escape,	"QUIT");

    m_title = "Sunday Lawn";

    m_logo.setTexture(m_game->assets().getTexture("Logo"));
    m_menuText.setFont(m_game->assets().getFont("MegaSurprise"));


    const size_t CHAR_SIZE{ 64 };
    m_menuText.setCharacterSize(CHAR_SIZE);

}


void Scene_Menu::registerItem(SceneID key, std::string item) {
    m_menuItems.push_back(std::make_pair(key, item));
}


void Scene_Menu::update(sf::Time dt) {
    m_entityManager.update();
}


void Scene_Menu::sRender() {

    m_background.setTexture(m_game->assets().getTexture("Lawn"));

    sf::View view = m_game->window().getView();
    view.setCenter(m_game->window().getSize().x / 2.f, m_game->window().getSize().y / 2.f);
    m_game->window().setView(view);

    m_background.setPosition(0.f, 0.f);

    static const sf::Color selectedColor(255, 164, 032);
    static const sf::Color normalColor(255, 255, 255);
    static const sf::Color backgroundColor(34,139,34);

    sf::Text footer("UP: W    DOWN: S   PLAY: ENTER    QUIT: ESC",
                    m_game->assets().getFont("MegaSurprise"),
                    20);
    footer.setFillColor(normalColor);
    centerOrigin(footer);
    footer.setPosition(view.getCenter().x, 920);

    m_game->window().clear(backgroundColor);
    m_game->window().draw(m_background);

    m_menuText.setFillColor(normalColor);
    m_menuText.setString(m_title);
    centerOrigin(m_menuText);

    m_menuText.setPosition(view.getCenter().x, view.getSize().y / 4);

    m_game->window().draw(m_menuText);
    
    centerOrigin(m_logo);
    m_logo.setPosition(view.getCenter());
    
    m_game->window().draw(m_logo);
    

    for (size_t i{ 0 }; i < m_menuItems.size(); ++i)
    {
        m_menuText.setFillColor((i == m_menuIndex ? selectedColor : normalColor));
        centerOrigin(m_menuText);
        m_menuText.setPosition(view.getCenter().x + (160* i), view.getCenter().y + m_logo.getLocalBounds().height);
        m_menuText.setString(m_menuItems.at(i).second);
        m_game->window().draw(m_menuText);
    }

    m_game->window().draw(footer);

}


void Scene_Menu::sDoAction(const Action &action) {
    if (action.type() == "START")
    {
        if (action.name() == "RIGHT")
        {
            m_menuIndex = (m_menuIndex + m_menuItems.size() - 1) % m_menuItems.size();
            playToggle();
        }
        else if (action.name() == "LEFT")
        {
            m_menuIndex = (m_menuIndex + 1) % m_menuItems.size();
            playToggle();
        }
        // TODO generalize
        else if (action.name() == "PLAY")
        {
           m_game->changeScene( m_menuItems.at(m_menuIndex).first );
           playToggle();
        }
        else if (action.name() == "QUIT")
        {
            onEnd();
        }
    }
}



void Scene_Menu::onEnd()
{
    m_game->window().close();
}

void Scene_Menu::playToggle()
{
    SoundPlayer::getInstance().play("toggle");
}