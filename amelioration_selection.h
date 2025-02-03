#include <SDL/SDL.h>
#include <SDL/SDL_ttf.h>
#include <stdbool.h>
#include <stdio.h>

#define LARGEUR 800
#define HAUTEUR 600

// Fonction pour afficher l'écran de sélection des améliorations
int amelioration(SDL_Surface* screen, bool vague_complete) {
    if (!vague_complete) {
        return -1; // Ne pas afficher si la vague n'est pas terminée
    }

    if (TTF_Init() == -1) {
        printf("Erreur SDL_ttf : %s\n", TTF_GetError());
        return -1;
    }
    
    const char* font_paths[] = {"FreeSans.ttf", "arial.ttf", "C:\\Windows\\Fonts\\arial.ttf"};
    TTF_Font* font = NULL;
    for (int i = 0; i < 3; i++) {
        font = TTF_OpenFont(font_paths[i], 36);
        if (font) {
            printf("Police chargée : %s\n", font_paths[i]);
            break;
        }
    }
    if (!font) {
        printf("Erreur chargement de la police : %s\n", TTF_GetError());
        TTF_Quit();
        return -1;
    }
    
    SDL_Color blanc = {255, 255, 255};
    SDL_Surface* title = TTF_RenderText_Solid(font, "Choisissez une amélioration", blanc);
    SDL_Surface* shieldText = TTF_RenderText_Solid(font, "Bouclier", blanc);
    SDL_Surface* missileText = TTF_RenderText_Solid(font, "Missile Puissant", blanc);
    
    SDL_Rect titlePos = {LARGEUR / 2 - title->w / 2, 100};
    SDL_Rect shieldButtonPos = {LARGEUR / 3 - shieldText->w / 2, 300};
    SDL_Rect missileButtonPos = {2 * LARGEUR / 3 - missileText->w / 2, 300};
    SDL_Rect shieldButtonRect = {shieldButtonPos.x - 20, shieldButtonPos.y - 10, shieldText->w + 40, shieldText->h + 20};
    SDL_Rect missileButtonRect = {missileButtonPos.x - 20, missileButtonPos.y - 10, missileText->w + 40, missileText->h + 20};
    
    bool running = true;
    SDL_Event event;
    int choix = -1; // -1 = aucun choix, 0 = bouclier, 1 = missile puissant

    while (running) {
        SDL_FillRect(screen, NULL, SDL_MapRGB(screen->format, 0, 0, 0));
        SDL_BlitSurface(title, NULL, screen, &titlePos);
        
        SDL_FillRect(screen, &shieldButtonRect, SDL_MapRGB(screen->format, 0, 255, 0));
        SDL_BlitSurface(shieldText, NULL, screen, &shieldButtonPos);
        
        SDL_FillRect(screen, &missileButtonRect, SDL_MapRGB(screen->format, 255, 0, 0));
        SDL_BlitSurface(missileText, NULL, screen, &missileButtonPos);
        
        SDL_Flip(screen);
        
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT) {
                running = false;
                return -1;
            }
            if (event.type == SDL_MOUSEBUTTONDOWN) {
                int x = event.button.x;
                int y = event.button.y;
                if (x >= shieldButtonRect.x && x <= shieldButtonRect.x + shieldButtonRect.w &&
                    y >= shieldButtonRect.y && y <= shieldButtonRect.y + shieldButtonRect.h) {
                    choix = 0;
                    running = false;
                }
                if (x >= missileButtonRect.x && x <= missileButtonRect.x + missileButtonRect.w &&
                    y >= missileButtonRect.y && y <= missileButtonRect.y + missileButtonRect.h) {
                    choix = 1;
                    running = false;
                }
            }
        }
        SDL_Delay(16);
    }
    
    SDL_FreeSurface(title);
    SDL_FreeSurface(shieldText);
    SDL_FreeSurface(missileText);
    TTF_CloseFont(font);
    TTF_Quit();
    
    return choix;
}
