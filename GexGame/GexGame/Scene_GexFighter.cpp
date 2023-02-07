//
// Created by David Burchill on 2022-11-09.
//

#include "Scene_GexFighter.h"
#include "Entity.h"
#include "MusicPlayer.h"
#include "SoundPlayer.h"

#include <fstream>
#include <iostream>
#include <random>


namespace {
    std::random_device rd;
    std::mt19937 rng(rd());
}


Scene_GexFighter::Scene_GexFighter(GameEngine *gameEngine, const std::string &configPath)
        : Scene(gameEngine), m_worldView(gameEngine->window().getDefaultView()), m_worldBounds({0, 0}, {0, 0}) {


}


void Scene_GexFighter::loadFromFile(const std::string &configPath) {
    
}


void Scene_GexFighter::init(const std::string &configPath) {

}


void Scene_GexFighter::keepEntitiesInBounds() {

}


void Scene_GexFighter::onEnd() {

}


void Scene_GexFighter::sMovement(sf::Time dt) {
    
}


void Scene_GexFighter::playerMovement() {



}


void Scene_GexFighter::sCollisions() {
    checkMissileCollision();
    checkBulletCollision();
    checkPlaneCollision();
    checkPickUpCollision();
}


void Scene_GexFighter::checkIfDead(NttPtr e) {

   
}

void Scene_GexFighter::checkPickUpCollision()
{

    
}

bool Scene_GexFighter::isGameWon()
{
    return true;
}



void Scene_GexFighter::checkPlaneCollision() {// check for plane collision

    
}


void Scene_GexFighter::checkBulletCollision() {

    
}


void Scene_GexFighter::checkMissileCollision() {// missiles
    
}


void Scene_GexFighter::sUpdate(sf::Time dt) {

}


void Scene_GexFighter::registerActions() {

    
}


void Scene_GexFighter::spawnPlayer() {
    

}


void Scene_GexFighter::drawAABB() {

   

}


void Scene_GexFighter::adjustPlayer() {
    


}


void Scene_GexFighter::checkPlayerState() {// set the player state
    
}


sf::FloatRect Scene_GexFighter::getViewBounds() {
    return sf::FloatRect();
}


void Scene_GexFighter::update() 
{

}


void Scene_GexFighter::sDoAction(const Action &action) {

    

}


void Scene_GexFighter::sRender() {

    


}


void Scene_GexFighter::sAnimation(sf::Time dt) {

    
}


void Scene_GexFighter::fireBullet() {
}


void Scene_GexFighter::createBullet(sf::Vector2f pos, bool isEnemy) {
    
}


void Scene_GexFighter::sGunUpdate(sf::Time dt) {
    
}


void Scene_GexFighter::fireMissile() {

    
}


sf::Vector2f Scene_GexFighter::findClosestEnemy(sf::Vector2f mPos) {
    return sf::Vector2f();
}


void Scene_GexFighter::sGuideMissiles(sf::Time dt) {

    
}


void Scene_GexFighter::spawnEnemies(std::string type, float offset, size_t numPlanes) {

    

}

void Scene_GexFighter::spawnPickUp(NttPtr e, int type)
{
    


}


void Scene_GexFighter::spawnEnemy(std::string type, sf::Vector2f pos) {

    
}


void Scene_GexFighter::spawnEnemies() {
    

}


void Scene_GexFighter::sAutoPilot(const sf::Time &dt) {// autopilot enemties
    
}

void Scene_GexFighter::sRemoveEntitiesOutOfGame()
{
}

