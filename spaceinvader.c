#include <SDL/SDL.h>
#include <SDL/SDL_image.h>
#include <SDL/SDL_mixer.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

// Dimensions de la fenêtre
#define LARGEUR 800
#define HAUTEUR 600

// Paramètres du vaisseau (joueur)
#define LARGEUR_VAISSEAU 30
#define HAUTEUR_VAISSEAU 20
#define VITESSE_VAISSEAU 10

// Paramètres des ennemis
#define NB_ENNEMIS_LIGNE 10
#define NB_ENNEMIS_COLONNE 3
#define LARGEUR_ENNEMI 30
#define HAUTEUR_ENNEMI 30
#define VITESSE_ENNEMI 2
#define VITESSE_DESCENTE 5

// Paramètres des tirs
#define LARGEUR_TIR 8
#define HAUTEUR_TIR 8
#define VITESSE_TIR 15
#define VITESSE_TIR_ENNEMI 5
#define CHANCE_TIR_ENNEMI 0.01

// Initialisation des variables globales
int score = 0;
int pointDeVie = 10;
bool gameOver = false;

// Structure des entités
typedef struct {
    int x, y;
    bool actif;
    SDL_Surface* surface;
} Entity;

// Fonction pour charger les images
SDL_Surface* chargerImage(const char* chemin) {
    SDL_Surface* surface = IMG_Load(chemin);
    if (!surface) {
        printf("Erreur de chargement d'image : %s\n", IMG_GetError());
        return NULL;
    }
    return surface;
}

// Fonction pour vérifier les collisions
bool verifierCollision(Entity a, int largeurA, int hauteurA, Entity b, int largeurB, int hauteurB) {
    return (a.x < b.x + largeurB &&
            a.x + largeurA > b.x &&
            a.y < b.y + hauteurB &&
            a.y + hauteurA > b.y);
}

// Fonction principale du jeu
void gameLoop(SDL_Surface* screen) {
    bool running = true;
    SDL_Event event;

    // Chargement des images
    SDL_Surface* fond = chargerImage("Decor.png");
    SDL_Surface* vaisseau = chargerImage("Joueur.png");
    SDL_Surface* ennemiSurface = chargerImage("Ennemi1.png");
    SDL_Surface* tirSurface = chargerImage("tir.png");
    SDL_Surface* tirEnnemiSurface = chargerImage("tirennemi.png");

    if (!fond || !vaisseau || !ennemiSurface || !tirSurface || !tirEnnemiSurface) {
        printf("Erreur : Une ou plusieurs images n'ont pas pu être chargées.\n");
        if (fond) SDL_FreeSurface(fond);
        if (vaisseau) SDL_FreeSurface(vaisseau);
        if (ennemiSurface) SDL_FreeSurface(ennemiSurface);
        if (tirSurface) SDL_FreeSurface(tirSurface);
        if (tirEnnemiSurface) SDL_FreeSurface(tirEnnemiSurface);
        return;
    }

    Mix_Chunk* laser_fx = Mix_LoadWAV("laser.wav");
    if (!laser_fx) {
        printf("Erreur lors du chargement du son laser : %s\n", Mix_GetError());
        SDL_FreeSurface(fond);
        SDL_FreeSurface(vaisseau);
        SDL_FreeSurface(ennemiSurface);
        SDL_FreeSurface(tirSurface);
        SDL_FreeSurface(tirEnnemiSurface);
        return;
    }
    Mix_VolumeChunk(laser_fx, MIX_MAX_VOLUME / 2);

    // Position initiale du vaisseau
    Entity joueur = {LARGEUR / 2 - LARGEUR_VAISSEAU / 2, HAUTEUR - HAUTEUR_VAISSEAU - 10, true, vaisseau};

    // Initialisation des ennemis
    Entity ennemis[NB_ENNEMIS_LIGNE][NB_ENNEMIS_COLONNE];
    for (int i = 0; i < NB_ENNEMIS_LIGNE; i++) {
        for (int j = 0; j < NB_ENNEMIS_COLONNE; j++) {
            ennemis[i][j].x = i * (LARGEUR_ENNEMI + 10) + 100;
            ennemis[i][j].y = j * (HAUTEUR_ENNEMI + 10) + 100;
            ennemis[i][j].actif = true;
            ennemis[i][j].surface = ennemiSurface;
        }
    }

    // Initialisation du tir du joueur
    Entity tir = {-100, -100, false, tirSurface};

    // Initialisation des tirs ennemis
    Entity tirsEnnemis[NB_ENNEMIS_LIGNE];
    for (int i = 0; i < NB_ENNEMIS_LIGNE; i++) {
        tirsEnnemis[i].x = -100;
        tirsEnnemis[i].y = -100;
        tirsEnnemis[i].actif = false;
        tirsEnnemis[i].surface = tirEnnemiSurface;
    }

    // Variables pour déplacement des ennemis
    bool directionDroite = true;

    // Boucle principale
    while (running) {
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT) {
                running = false;
            }
        }

        // Déplacements du vaisseau
        Uint8* keystates = SDL_GetKeyState(NULL);
        if (keystates[SDLK_LEFT] && joueur.x > 0) joueur.x -= VITESSE_VAISSEAU;
        if (keystates[SDLK_RIGHT] && joueur.x < LARGEUR - LARGEUR_VAISSEAU) joueur.x += VITESSE_VAISSEAU;

        // Tir avec la barre espace
        if (keystates[SDLK_SPACE] && !tir.actif) {
            tir.actif = true;
            tir.x = joueur.x + LARGEUR_VAISSEAU / 2 - LARGEUR_TIR / 2;
            tir.y = joueur.y - HAUTEUR_TIR;
            Mix_PlayChannel(-1, laser_fx, 0);
        }

        // Mise à jour du tir du joueur
        if (tir.actif) {
            tir.y -= VITESSE_TIR;
            if (tir.y + HAUTEUR_TIR < 0) {
                tir.actif = false;
            }
            for (int i = 0; i < NB_ENNEMIS_LIGNE; i++) {
                for (int j = 0; j < NB_ENNEMIS_COLONNE; j++) {
                    if (ennemis[i][j].actif && verifierCollision(tir, LARGEUR_TIR, HAUTEUR_TIR, ennemis[i][j], LARGEUR_ENNEMI, HAUTEUR_ENNEMI)) {
                        ennemis[i][j].actif = false;
                        tir.actif = false;
                        score += 10;
                    }
                }
            }
        }

        // Déplacement des ennemis
        for (int i = 0; i < NB_ENNEMIS_LIGNE; i++) {
            for (int j = 0; j < NB_ENNEMIS_COLONNE; j++) {
                if (ennemis[i][j].actif) {
                    ennemis[i][j].x += directionDroite ? VITESSE_ENNEMI : -VITESSE_ENNEMI;
                }
            }
        }

        // Changer de direction et descendre
        if (directionDroite && ennemis[NB_ENNEMIS_LIGNE - 1][0].x + LARGEUR_ENNEMI >= LARGEUR) {
            directionDroite = false;
            for (int i = 0; i < NB_ENNEMIS_LIGNE; i++) {
                for (int j = 0; j < NB_ENNEMIS_COLONNE; j++) {
                    ennemis[i][j].y += VITESSE_DESCENTE;
                }
            }
        } else if (!directionDroite && ennemis[0][0].x <= 0) {
            directionDroite = true;
            for (int i = 0; i < NB_ENNEMIS_LIGNE; i++) {
                for (int j = 0; j < NB_ENNEMIS_COLONNE; j++) {
                    ennemis[i][j].y += VITESSE_DESCENTE;
                }
            }
        }

        // Mise à jour des tirs ennemis
        for (int i = 0; i < NB_ENNEMIS_LIGNE; i++) {
            if (!tirsEnnemis[i].actif && ennemis[i][0].actif && ((rand() % 1000) / 1000.0) < CHANCE_TIR_ENNEMI) {
                tirsEnnemis[i].actif = true;
                tirsEnnemis[i].x = ennemis[i][0].x + LARGEUR_ENNEMI / 2 - LARGEUR_TIR / 2;
                tirsEnnemis[i].y = ennemis[i][0].y + HAUTEUR_ENNEMI;
            }

            if (tirsEnnemis[i].actif) {
                tirsEnnemis[i].y += VITESSE_TIR_ENNEMI;
                if (tirsEnnemis[i].y > HAUTEUR) {
                    tirsEnnemis[i].actif = false;
                }

                // Collision avec le joueur
                if (verifierCollision(tirsEnnemis[i], LARGEUR_TIR, HAUTEUR_TIR, joueur, LARGEUR_VAISSEAU, HAUTEUR_VAISSEAU)) {
                    tirsEnnemis[i].actif = false;
                    pointDeVie--;
                    if (pointDeVie <= 0) {
                        gameOver = true;
                        running = false;
                    }
                }
            }
        }

        // Rendu de l'écran
        SDL_FillRect(screen, NULL, SDL_MapRGB(screen->format, 0, 0, 0)); // Noir
        SDL_BlitSurface(fond, NULL, screen, NULL);

        // Rendu du vaisseau
        SDL_Rect rectJoueur = {joueur.x, joueur.y, LARGEUR_VAISSEAU, HAUTEUR_VAISSEAU};
        SDL_BlitSurface(joueur.surface, NULL, screen, &rectJoueur);

        // Rendu des ennemis
        for (int i = 0; i < NB_ENNEMIS_LIGNE; i++) {
            for (int j = 0; j < NB_ENNEMIS_COLONNE; j++) {
                if (ennemis[i][j].actif) {
                    SDL_Rect rectEnnemi = {ennemis[i][j].x, ennemis[i][j].y, LARGEUR_ENNEMI, HAUTEUR_ENNEMI};
                    SDL_BlitSurface(ennemis[i][j].surface, NULL, screen, &rectEnnemi);
                }
            }
        }

        // Rendu des tirs du joueur
        if (tir.actif) {
            SDL_Rect rectTir = {tir.x, tir.y, LARGEUR_TIR, HAUTEUR_TIR};
            SDL_BlitSurface(tir.surface, NULL, screen, &rectTir);
        }

        // Rendu des tirs ennemis
        for (int i = 0; i < NB_ENNEMIS_LIGNE; i++) {
            if (tirsEnnemis[i].actif) {
                SDL_Rect rectTirEnnemi = {tirsEnnemis[i].x, tirsEnnemis[i].y, LARGEUR_TIR, HAUTEUR_TIR};
                SDL_BlitSurface(tirsEnnemis[i].surface, NULL, screen, &rectTirEnnemi);
            }
        }

        SDL_Flip(screen); // Mise à jour de l'écran
        SDL_Delay(16); // Limiter à ~60 FPS

        // Debug : Afficher les coordonnées et le score
        printf("Score: %d | Point de vie: %d\n", score, pointDeVie);
    }

    // Libération des ressources
    SDL_FreeSurface(fond);
    SDL_FreeSurface(vaisseau);
    SDL_FreeSurface(ennemiSurface);
    SDL_FreeSurface(tirSurface);
    SDL_FreeSurface(tirEnnemiSurface);
    Mix_FreeChunk(laser_fx);
}

int main(int argc, char* argv[]) {
    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO) < 0) {
        printf("Erreur SDL : %s\n", SDL_GetError());
        return 1;
    }

    if (IMG_Init(IMG_INIT_PNG) == 0) {
        printf("Erreur SDL_Image : %s\n", IMG_GetError());
        SDL_Quit();
        return 1;
    }

    if (Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 2048) < 0) {
        printf("Erreur SDL_Mixer : %s\n", Mix_GetError());
        IMG_Quit();
        SDL_Quit();
        return 1;
    }

    SDL_Surface* screen = SDL_SetVideoMode(LARGEUR, HAUTEUR, 32, SDL_HWSURFACE | SDL_DOUBLEBUF);
    if (!screen) {
        printf("Erreur lors de la création de la fenêtre : %s\n", SDL_GetError());
        Mix_CloseAudio();
        IMG_Quit();
        SDL_Quit();
        return 1;
    }

    srand(time(NULL)); // Initialisation de la graine pour les positions aléatoires

    gameLoop(screen);

    SDL_Quit();
    Mix_CloseAudio();
    IMG_Quit();

    return 0;
}
