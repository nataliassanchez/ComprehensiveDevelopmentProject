#pragma once

#include "Scene.h"


class Scene_Levels : public Scene
{
private:
	std::vector<std::pair<SceneID, std::string>>	m_menuItems;
	sf::Text					m_menuText;
	std::vector<std::string>	m_levelPaths;
	int							m_menuIndex{ 0 };
	std::string					m_title;
	sf::Sprite                  m_background;


	void init();
	void registerItem(SceneID key, std::string item);
	void onEnd() override;
public:

	Scene_Levels(GameEngine* gameEngine);

	void update() override;

	void sRender() override;
	void sDoAction(const Action& action) override;

};

