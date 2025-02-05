#include "page_accueil_sdl.h"
#include <SDL/SDL_ttf.h>
#include <stdio.h>

typedef enum {
    MENU_PLAY,
    MENU_OPTIONS,
    MENU_CUSTOMISATION,
    MENU_QUITTER,
    MENU_COUNT // Nombre total d'options
} MenuOption;

bool accueil(SDL_Surface* screen) {
    if (TTF_Init() == -1) {
        printf("Erreur SDL_ttf : %s\n", TTF_GetError());
        return false;
    }
    
    TTF_Font* font = TTF_OpenFont("C:/Windows/Fonts/arial.ttf", 48);
    if (!font) {
        printf("Erreur chargement de la police : %s\n", TTF_GetError());
        TTF_Quit();
        return false;
    }
    
    SDL_Color blanc = {255, 255, 255};
    SDL_Surface* title = TTF_RenderText_Solid(font, "SPACE INVADER", blanc);

    const char* options[MENU_COUNT] = { "PLAY", "OPTIONS", "CUSTOMISATION", "QUITTER" };
    SDL_Surface* optionTexts[MENU_COUNT];

    SDL_Rect titlePos = {400 - title->w / 2, 100};

    SDL_Rect optionPositions[MENU_COUNT];
    SDL_Rect optionRects[MENU_COUNT];

    for (int i = 0; i < MENU_COUNT; i++) {
        optionTexts[i] = TTF_RenderText_Solid(font, options[i], blanc);
        optionPositions[i].x = 400 - optionTexts[i]->w / 2;
        optionPositions[i].y = 200 + i * 80;

        optionRects[i].x = optionPositions[i].x - 20;
        optionRects[i].y = optionPositions[i].y - 10;
        optionRects[i].w = optionTexts[i]->w + 40;
        optionRects[i].h = optionTexts[i]->h + 20;
    }

    bool running = true;
    SDL_Event event;
    int selectedOption = 0;

    while (running) {
        SDL_FillRect(screen, NULL, SDL_MapRGB(screen->format, 0, 0, 0));
        SDL_BlitSurface(title, NULL, screen, &titlePos);

        // Afficher les options du menu
        for (int i = 0; i < MENU_COUNT; i++) {
            Uint32 buttonColor = (i == selectedOption) ? SDL_MapRGB(screen->format, 200, 0, 0) // Surbrillance rouge
                                                        : SDL_MapRGB(screen->format, 100, 100, 255); // Couleur normale
            SDL_FillRect(screen, &optionRects[i], buttonColor);
            SDL_BlitSurface(optionTexts[i], NULL, screen, &optionPositions[i]);
        }

        SDL_Flip(screen);

        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT) {
                running = false;
            }
            if (event.type == SDL_KEYDOWN) {
                if (event.key.keysym.sym == SDLK_DOWN) {
                    selectedOption = (selectedOption + 1) % MENU_COUNT;
                }
                if (event.key.keysym.sym == SDLK_UP) {
                    selectedOption = (selectedOption - 1 + MENU_COUNT) % MENU_COUNT;
                }
                if (event.key.keysym.sym == SDLK_RETURN) {
                    if (selectedOption == MENU_PLAY) {
                        return true;
                    } else if (selectedOption == MENU_QUITTER) {
                        running = false;
                    } else if (selectedOption == MENU_OPTIONS) {
                        printf("Affichage des options...\n"); // À implémenter
                    } else if (selectedOption == MENU_CUSTOMISATION) {
                        printf("Affichage de la customisation...\n"); // À implémenter
                    }
                }
            }
            if (event.type == SDL_MOUSEMOTION) {
                int x = event.motion.x, y = event.motion.y;
                for (int i = 0; i < MENU_COUNT; i++) {
                    if (x >= optionRects[i].x && x <= optionRects[i].x + optionRects[i].w &&
                        y >= optionRects[i].y && y <= optionRects[i].y + optionRects[i].h) {
                        selectedOption = i;
                    }
                }
            }
            if (event.type == SDL_MOUSEBUTTONDOWN) {
                int x = event.button.x, y = event.button.y;
                for (int i = 0; i < MENU_COUNT; i++) {
                    if (x >= optionRects[i].x && x <= optionRects[i].x + optionRects[i].w &&
                        y >= optionRects[i].y && y <= optionRects[i].y + optionRects[i].h) {
                        if (i == MENU_PLAY) {
                            return true;
                        } else if (i == MENU_QUITTER) {
                            running = false;
                        } else if (i == MENU_OPTIONS) {
                            printf("Affichage des options...\n"); // À implémenter
                        } else if (i == MENU_CUSTOMISATION) {
                            printf("Affichage de la customisation...\n"); // À implémenter
                        }
                    }
                }
            }
        }
        SDL_Delay(16);
    }
    
    SDL_FreeSurface(title);
    for (int i = 0; i < MENU_COUNT; i++) {
        SDL_FreeSurface(optionTexts[i]);
    }
    TTF_CloseFont(font);
    TTF_Quit();
    return false;
}
