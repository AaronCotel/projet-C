#include <SDL/SDL.h>
#include <SDL/SDL_ttf.h>
#include <stdbool.h>
#include <stdio.h>

#define LARGEUR 800
#define HAUTEUR 600

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
    SDL_Surface* playText = TTF_RenderText_Solid(font, "PLAY", blanc);
    
    SDL_Rect titlePos = {LARGEUR / 2 - title->w / 2, 150};
    SDL_Rect playButtonPos = {LARGEUR / 2 - playText->w / 2, 300};
    SDL_Rect playButtonRect = {playButtonPos.x - 20, playButtonPos.y - 10, playText->w + 40, playText->h + 20};
    
    bool running = true;
    SDL_Event event;
    while (running) {
        SDL_FillRect(screen, NULL, SDL_MapRGB(screen->format, 0, 0, 0));
        SDL_BlitSurface(title, NULL, screen, &titlePos);
        
        // Dessiner le bouton
        SDL_FillRect(screen, &playButtonRect, SDL_MapRGB(screen->format, 100, 100, 255));
        SDL_BlitSurface(playText, NULL, screen, &playButtonPos);
        
        SDL_Flip(screen);
        
        while (SDL_PollEvent(&event)) { // Écouter les événements
            if (event.type == SDL_QUIT) {
                running = false;
                return false;
            }
            if (event.type == SDL_MOUSEBUTTONDOWN) {
                int x = event.button.x;
                int y = event.button.y;
                if (x >= playButtonRect.x && x <= playButtonRect.x + playButtonRect.w &&
                    y >= playButtonRect.y && y <= playButtonRect.y + playButtonRect.h) {
                    return true;
                }
            }
        }
        SDL_Delay(16); // Pour éviter l'utilisation excessive du CPU
    }
    
    SDL_FreeSurface(title);
    SDL_FreeSurface(playText);
    TTF_CloseFont(font);
    TTF_Quit();
    return false;
}
