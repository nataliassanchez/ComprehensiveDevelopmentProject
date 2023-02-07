////////////////////////////////////////////////////////////////////////////////////////////////////////////
// 
//  Assignment:     PROG1266 Assignment xxx
//  Instructor:     David Burchill
//  Year / Term:    Fall 2022
//  File name:      sfml.cpp
// 
//  Student name:   Natalia Sanchez Castillo
//  Student email:  nataliasanchezc0@gmail.com
// 
//     I certify that this work is my work only, any work copied from Stack Overflow, textbooks, 
//     or elsewhere is properly cited. 
//
// ////////////////////////////////////////////////////////////////////////////////////////////////////////////



#include "GameEngine.h"
#include "Utilities.h"
#include <SFML/System.hpp>


int main() {

    GameEngine game("../config.txt");
    game.run();

    return 0;
}