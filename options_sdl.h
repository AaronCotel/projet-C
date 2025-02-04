#ifndef OPTIONS_SDL_H
#define OPTIONS_SDL_H

#include <SDL/SDL.h>
#include <SDL/SDL_ttf.h>
#include <SDL/SDL_mixer.h>
#include <stdio.h>
#include <stdbool.h>

#define LARGEUR 800
#define HAUTEUR 600
#define VOLUME_STEP 8  // Incrément de volume
#define CONFIG_FILE "config.txt"

// Fonction pour sauvegarder le volume dans un fichier
void sauvegarderVolume(int volume) {
    FILE* fichier = fopen(CONFIG_FILE, "w");
    if (fichier) {
        fprintf(fichier, "%d\n", volume);
        fclose(fichier);
    } else {
        printf("Erreur : Impossible de sauvegarder le volume.\n");
    }
}

// Fonction pour charger le volume depuis le fichier
int chargerVolume() {
    FILE* fichier = fopen(CONFIG_FILE, "r");
    int volume = MIX_MAX_VOLUME / 2; // Valeur par défaut à 50%
    
    if (fichier) {
        fscanf(fichier, "%d", &volume);
        fclose(fichier);
    } else {
        printf("Aucun fichier de configuration trouvé, utilisation du volume par défaut.\n");
    }

    return volume;
}

// Fonction pour afficher le menu des options et régler le volume
static inline void options(SDL_Surface* screen) {
    bool running = true;
    SDL_Event event;
    
    int volume = chargerVolume(); // Charger le volume sauvegardé
    Mix_Volume(-1, volume);
    Mix_VolumeMusic(volume);

    // Initialisation de la police
    if (TTF_Init() == -1) {
        printf("Erreur SDL_ttf : %s\n", TTF_GetError());
        return;
    }
    TTF_Font* font = TTF_OpenFont("C:/Windows/Fonts/arial.ttf", 28);
    if (!font) {
        printf("Erreur chargement de la police : %s\n", TTF_GetError());
        return;
    }

    SDL_Color blanc = {255, 255, 255};

    while (running) {
        SDL_FillRect(screen, NULL, SDL_MapRGB(screen->format, 50, 50, 50)); // Fond gris

        // Affichage de la barre de volume
        SDL_Rect volumeBar = {200, 300, (volume * 400) / MIX_MAX_VOLUME, 50}; // Largeur proportionnelle au volume
        SDL_FillRect(screen, &volumeBar, SDL_MapRGB(screen->format, 0, 255, 0)); // Barre verte

        // Création des textes d'indication
        char volumeText[50];
        sprintf(volumeText, "Volume : %d%%", (volume * 100) / MIX_MAX_VOLUME);
        SDL_Surface* volumeSurface = TTF_RenderText_Solid(font, volumeText, blanc);
        SDL_Surface* instructionSurface = TTF_RenderText_Solid(font, "Haut/Bas pour changer le volume", blanc);
        SDL_Surface* exitSurface = TTF_RenderText_Solid(font, "Echap pour revenir au menu", blanc);

        SDL_Rect volumePos = {LARGEUR / 2 - volumeSurface->w / 2, 200};
        SDL_Rect instructionPos = {LARGEUR / 2 - instructionSurface->w / 2, 400};
        SDL_Rect exitPos = {LARGEUR / 2 - exitSurface->w / 2, 450};

        SDL_BlitSurface(volumeSurface, NULL, screen, &volumePos);
        SDL_BlitSurface(instructionSurface, NULL, screen, &instructionPos);
        SDL_BlitSurface(exitSurface, NULL, screen, &exitPos);

        SDL_FreeSurface(volumeSurface);
        SDL_FreeSurface(instructionSurface);
        SDL_FreeSurface(exitSurface);

        // Détection des événements
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT) {
                running = false;
            } 
            else if (event.type == SDL_KEYDOWN) {
                switch (event.key.keysym.sym) {
                    case SDLK_UP:  // Augmenter le volume
                        if (volume < MIX_MAX_VOLUME) volume += VOLUME_STEP;
                        Mix_Volume(-1, volume);
                        Mix_VolumeMusic(volume);
                        sauvegarderVolume(volume);
                        break;
                    case SDLK_DOWN:  // Diminuer le volume
                        if (volume > 0) volume -= VOLUME_STEP;
                        Mix_Volume(-1, volume);
                        Mix_VolumeMusic(volume);
                        sauvegarderVolume(volume);
                        break;
                    case SDLK_ESCAPE:  // Quitter et revenir au menu principal
                        running = false;
                        break;
                }
            }
        }

        SDL_Flip(screen); // Rafraîchir l'affichage
        SDL_Delay(50); // Réduction de la charge CPU
    }

    TTF_CloseFont(font);
    TTF_Quit();
}

#endif
