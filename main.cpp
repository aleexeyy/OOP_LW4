#include "classes.cpp"
#include "init.h"


int main() {
    if (init() != 0) {
        return 1;
    }
    Menu menu;
    Field* gameField = nullptr;
    bool quit = false;

    while (!quit) {
        int mouseX, mouseY;
        SDL_GetMouseState(&mouseX, &mouseY);

        while (SDL_PollEvent(&event) != 0) {
            if (event.type == SDL_QUIT) {
                quit = true;
            } else if (event.type == SDL_MOUSEBUTTONDOWN) {
                switch (currentState) {
                    case MENU: {
                        std::array<int, 2> new_state = menu.handleEvent(mouseX, mouseY);
                        if (new_state[0] == -1 && new_state[1] == -1) {
                            quit = true;
                        } else if (new_state[1] != 10) {
                            gameField = new Field(new_state[0], new_state[1]);
                        }
                        break;
                    }
                    case GAME: {
                        gameField->handleEvent(mouseX, mouseY);
                        break;
                    }
                }
            } else if (event.type == SDL_TEXTINPUT && menu.get_nodesNumber().size() <= 1) {
                menu.addSymbol(event.text.text);
            } else if (event.type == SDL_KEYDOWN) {
                if (event.key.keysym.sym == SDLK_BACKSPACE && menu.get_nodesNumber().size() > 0) {
                    menu.deleteSymbol();
                }
            }
        }
        switch (currentState) {
            case MENU:
                menu.draw(renderer, mouseX, mouseY);
                break;
            case GAME: {
                gameField->draw(renderer, mouseX, mouseY);
                break;
            }
        }

        SDL_RenderPresent(renderer);
        if (currentState == GAME) {
            gameField->finishGame();
        }

    }
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
    delete gameField;

    return 0;
}
