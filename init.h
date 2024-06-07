#ifndef INIT_H
#define INIT_H

#include <SDL.h>
#include <iostream>
#include <SDL_ttf.h>
#include <string>

const int Height = 640;
const int Width = 800;
extern SDL_Renderer* renderer;
extern SDL_Window* window;
extern SDL_Event event;

enum GameState { MENU, GAME};
extern GameState currentState;

void DrawLine(SDL_Renderer* renderer, int x_from, int y_from, int x_to, int y_to, Uint8 r, Uint8 g, Uint8 b);
void DrawRectangle(SDL_Renderer* renderer, int x_from, int y_from, int x_to, int y_to, Uint8 r, Uint8 g, Uint8 b);
void DrawRectangle(SDL_Renderer* renderer, int x_from, int y_from, int x_to, int y_to, std::string text, Uint8 r, Uint8 g, Uint8 b);
void DrawFilledCircle(SDL_Renderer* renderer, int centreX, int centreY, int radius, Uint8 r, Uint8 g, Uint8 b);
void DrawCircle(SDL_Renderer * renderer, int centreX, int centreY, int radius);
int init();

#endif