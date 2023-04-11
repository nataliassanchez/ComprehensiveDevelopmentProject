//
// Created by David Burchill on 2022-10-03.
//

#include "GameEngine.h"
#include "Components.h"
#include "Entity.h"
#include "Scene_GexFighter.h"

#include <fstream>
#include <iostream>
#include <string>
#include <tuple>

#include "Scene_Game.h"
#include "Scene_Menu.h"
#include "MusicPlayer.h"
#include "SoundPlayer.h"
#include "Scene_Levels.h"
#include "Scene_Options.h"


const sf::Time GameEngine::TIME_PER_FRAME = sf::seconds((1.f / 60.f));


GameEngine::GameEngine(const std::string& configPath) {
    init(configPath);
    m_assets.loadFromFile(configPath);
    m_window.create(sf::VideoMode(m_windowSize.x, m_windowSize.y), "SUNDAY LAWN");

    m_statisticsText.setFont(m_assets.getFont("Arial"));
    m_statisticsText.setPosition(15.0f, 15.0f);
    m_statisticsText.setCharacterSize(15);

    createFactories();
    createMenu();
    m_currentScene = SceneID::MENU;
}


void GameEngine::createMenu() {

    // create the menu_scene and put in sceneMap
    auto menuScene = std::make_shared<Scene_Menu>(this);
    m_sceneMap[SceneID::MENU] = menuScene;

    // add items to menu_scene
    menuScene->registerItem(SceneID::LEVEL1, "Start");
    menuScene->registerItem(SceneID::OPTIONS, "Options");
    

}


void GameEngine::createFactories() {
    m_factories[SceneID::MENU] = std::function<Sptr()> (
            [this]() -> Sptr {
                return std::make_shared<Scene_Menu>(this);
            });

    m_factories[SceneID::LEVELS] = std::function<Sptr()>(
        [this]() -> Sptr {
            return std::make_shared<Scene_Levels>(this);
        });

    m_factories[SceneID::OPTIONS] = std::function<Sptr()>(
        [this]() -> Sptr {
            return std::make_shared<Scene_Options>(this);
        });

    m_factories[SceneID::LEVEL1] = std::function<Sptr()>(
        [this]() -> Sptr {
            return std::make_shared<Scene_Game>(this, "../assets/level1.txt");
        });
    m_factories[SceneID::LEVEL2] = std::function<Sptr()>(
        [this]() -> Sptr {
            return std::make_shared<Scene_Game>(this, "../assets/level2.txt");
        });
    m_factories[SceneID::LEVEL3] = std::function<Sptr()>(
        [this]() -> Sptr {
            return std::make_shared<Scene_Game>(this, "../assets/level3.txt");
        });
    m_factories[SceneID::LEVEL4] = std::function<Sptr()>(
        [this]() -> Sptr {
            return std::make_shared<Scene_Game>(this, "../assets/level4.txt");
        });
    m_factories[SceneID::LEVEL5] = std::function<Sptr()>(
        [this]() -> Sptr {
            return std::make_shared<Scene_Game>(this, "../assets/level5.txt");
        });
}


void GameEngine::init(const std::string &configPath) {

    // get the window size
    std::ifstream config(configPath);
    if (config.fail()) {
        std::cerr << "Open file " << configPath << " failed\n";
        config.close();
        exit(1);
    }

    std::string token{""};
    config >> token;
    while (config) {
        while (config) {
            if (token == "Window") {
                config >> m_windowSize.x >> m_windowSize.y;
            }
            else if (token == "Sound") {
                std::string key, path;
                config >> key >> path;

                SoundPlayer::getInstance().loadBuffer(key, path);
            }
            config >> token;
        }
    }
    config.close();
}



void GameEngine::run() {
    
    MusicPlayer::getInstance().play("menuTheme2");
    MusicPlayer::getInstance().setVolume(15);

    const sf::Time SPF = sf::seconds(1.0f / 60.f);  // seconds per frame for 60 fps 

    sf::Clock clock;
    sf::Time timeSinceLastUpdate = sf::Time::Zero;

    // as is from youtube video
    while (isRunning())
    {
        sUserInput();								// get user input

        timeSinceLastUpdate += clock.restart();
        while (timeSinceLastUpdate > SPF)
        {
            currentScene()->update(SPF);				// update world
            timeSinceLastUpdate -= SPF;
        }

        currentScene()->sRender();					// render world

        m_window.setView(m_window.getDefaultView());
        m_window.draw(m_statisticsText);
        m_window.display();
    }
}



void GameEngine::sUserInput()
{
    sf::Event event;
    while (m_window.pollEvent(event))
    {
        if (event.type == sf::Event::Closed)
            quit();

        if (event.type == sf::Event::KeyPressed || event.type == sf::Event::KeyReleased)
        {
            if (currentScene()->getActionMap().contains(event.key.code))
            {
                std::string actionType = (event.type == sf::Event::KeyPressed) ? "START" : "END";
                currentScene()->doAction(Action(currentScene()->getActionMap().at(event.key.code), actionType));
            }
        }
    }
}


void GameEngine::updateStatistics(sf::Time dt) {
    m_statisticsUpdateTime += dt;
    m_statisticsNumFrames += 1;
    if (m_statisticsUpdateTime >= sf::seconds(1.0f)) {
        m_statisticsText.setString("FPS: " + std::to_string(m_statisticsNumFrames));
        m_statisticsUpdateTime -= sf::seconds(1.f);
        m_statisticsNumFrames = 0;
    }
}


void GameEngine::changeScene(SceneID id,  bool endCurrentScene) {

    if (id == SceneID::NONE)
        return;

    if (endCurrentScene)
        m_sceneMap.erase(m_currentScene);

    if (!m_sceneMap.contains(id))
        m_sceneMap[id] = m_factories[id]();

    m_currentScene = id;
}

SceneID GameEngine::getCurrentScene()
{
    return m_currentScene;
}


std::shared_ptr<Scene> GameEngine::currentScene() {
    return m_sceneMap.at(m_currentScene);
}


void GameEngine::quit() {
    m_window.close();
}


sf::RenderWindow &GameEngine::window() {
    return m_window;
}


const Assets &GameEngine::assets() const {
    return m_assets;
}


bool GameEngine::isRunning() {
    return (m_isRunning && m_window.isOpen());
}

void  GameEngine::quitLevel() {
    changeScene(SceneID::MENU, true);
}

void    GameEngine::backLevel() {
    changeScene(SceneID::MENU, false);
}

