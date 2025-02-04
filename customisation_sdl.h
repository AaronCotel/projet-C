#ifndef CUSTOMISATION_SDL_H
#define CUSTOMISATION_SDL_H

#include <SDL/SDL.h>
#include <stdio.h>
#include <stdbool.h>

static inline void customisation(SDL_Surface* screen) {
    bool running = true;
    SDL_Event event;

    while (running) {
        SDL_FillRect(screen, NULL, SDL_MapRGB(screen->format, 0, 0, 255)); // Fond bleu
        printf("Affichage du menu de customisation...\n");

        // Gestion des événements pour quitter le menu
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT) {
                running = false;
            } 
            else if (event.type == SDL_KEYDOWN) {
                if (event.key.keysym.sym == SDLK_ESCAPE) { // Touche Échap pour quitter
                    running = false;
                }
            }
        }

        SDL_Flip(screen); // Mise à jour de l'affichage
        SDL_Delay(50); // Évite l'utilisation excessive du CPU
    }
}

#endif
