#include "init.h"

SDL_Window* window = nullptr;
SDL_Renderer* renderer = nullptr;
SDL_Event event;
GameState currentState = MENU;

void DrawLine(SDL_Renderer* renderer, int x_from, int y_from, int x_to, int y_to, Uint8 r, Uint8 g, Uint8 b) {
    SDL_SetRenderDrawColor(renderer, r, g, b, SDL_ALPHA_OPAQUE);
    SDL_RenderDrawLine(renderer, x_from, y_from, x_to, y_to);
}
void DrawRectangle(SDL_Renderer* renderer, int x_from, int y_from, int x_to, int y_to, Uint8 r, Uint8 g, Uint8 b) {
    SDL_SetRenderDrawColor(renderer, r, g, b, SDL_ALPHA_OPAQUE);
    SDL_RenderDrawLine(renderer, x_from, y_from, x_to, y_from);
    SDL_RenderDrawLine(renderer, x_to, y_from, x_to, y_to);
    SDL_RenderDrawLine(renderer, x_to, y_to, x_from, y_to);
    SDL_RenderDrawLine(renderer, x_from, y_to, x_from, y_from);
}
void DrawRectangle(SDL_Renderer* renderer, int x_from, int y_from, int x_to, int y_to, std::string text, Uint8 r, Uint8 g, Uint8 b) {
    if (TTF_Init() == -1) {
        std::cerr << "TTF_Init: " << TTF_GetError() << std::endl;
        return;
    }
    TTF_Font* Sans = TTF_OpenFont("/Users/alex/Developer/C++/OOP/LabWork4/open-sans/OpenSans-Regular.ttf", 24);
    if (Sans == NULL) {
        std::cerr << "TTF_OpenFont: " << TTF_GetError() << std::endl;
        return;
    }
    SDL_Color White = {255, 255, 255};
    SDL_Surface* surfaceMessage = TTF_RenderText_Solid(Sans, text.c_str(), White); 
    SDL_Texture* Message = SDL_CreateTextureFromSurface(renderer, surfaceMessage);
    SDL_Rect Message_rect;
    Message_rect.x = x_from+10;
    Message_rect.y = y_from + 10;
    Message_rect.w = (x_to - x_from - 20);
    Message_rect.h = (y_to - y_from - 20);
    SDL_RenderCopy(renderer, Message, NULL, &Message_rect);

    SDL_FreeSurface(surfaceMessage);
    SDL_DestroyTexture(Message);
    TTF_CloseFont(Sans);
    TTF_Quit();
    DrawRectangle(renderer, x_from, y_from, x_to, y_to, r, g, b);
}
void DrawFilledCircle(SDL_Renderer* renderer, int centreX, int centreY, int radius, Uint8 r, Uint8 g, Uint8 b) {
    SDL_SetRenderDrawColor(renderer, r, g, b, SDL_ALPHA_OPAQUE);
    for (int x = -radius; x <= radius; ++x) {
        for (int y = -radius; y <= radius; ++y) {
            if (x * x + y * y <= radius * radius) {
                SDL_RenderDrawPoint(renderer, centreX + x, centreY + y);
            }
        }
    }
}

void DrawCircle(SDL_Renderer * renderer, int centreX, int centreY, int radius) {
   const int32_t diameter = (radius * 2);

   int32_t x = (radius - 1);
   int32_t y = 0;
   int32_t tx = 1;
   int32_t ty = 1;
   int32_t error = (tx - diameter);

   while (x >= y)
   {
      SDL_RenderDrawPoint(renderer, centreX + x, centreY - y);
      SDL_RenderDrawPoint(renderer, centreX + x, centreY + y);
      SDL_RenderDrawPoint(renderer, centreX - x, centreY - y);
      SDL_RenderDrawPoint(renderer, centreX - x, centreY + y);
      SDL_RenderDrawPoint(renderer, centreX + y, centreY - x);
      SDL_RenderDrawPoint(renderer, centreX + y, centreY + x);
      SDL_RenderDrawPoint(renderer, centreX - y, centreY - x);
      SDL_RenderDrawPoint(renderer, centreX - y, centreY + x);

      if (error <= 0)
      {
         ++y;
         error += ty;
         ty += 2;
      }

      if (error > 0)
      {
         --x;
         tx += 2;
         error += (tx - diameter);
      }
   }
}

int init() {
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        SDL_Log("SDL could not initialize! SDL_Error: %s\n", SDL_GetError());
        return 1;
    }
    window = SDL_CreateWindow("SDL Tutorial", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, Width, Height, SDL_WINDOW_SHOWN);
    if (window == nullptr) {
        SDL_Log("Window could not be created! SDL_Error: %s\n", SDL_GetError());
        return 1;
    }
    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    if (renderer == nullptr) {
        SDL_Log("Renderer could not be created! SDL_Error: %s\n", SDL_GetError());
        SDL_DestroyWindow(window);
        SDL_Quit();
        return 1;
    }
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
    SDL_RenderClear(renderer);
    return 0;
}



