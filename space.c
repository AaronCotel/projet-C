#include <SDL/SDL.h>
#include <SDL/SDL_image.h>
#include <SDL/SDL_mixer.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "page_accueil_sdl.h"
#include "amelioration_selection.h"
#include "options_sdl.h"
#include "customisation_sdl.h"


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
#define VITESSE_DESCENTE 20
#define ESPACEMENT_ENNEMI 40

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
int vagueActuelle = 1; // De 1 à 5 pour les rounds ennemis, 6 pour le boss
int totalVagues = 5;   // Nombre total de vagues avant le boss
int bouclier = 0; // 0 = pas de bouclier, >0 = nombre de tirs qu'il peut encaisser
bool missileAmeliore = false; // False = missile normal, True = missile qui détruit 3 ennemis


// Structure des entités
typedef struct {
    int x, y;
    bool actif;
    bool directionDroite; // Direction individuelle
    SDL_Surface* surface;
} Entity;

Entity ennemis[NB_ENNEMIS_LIGNE][NB_ENNEMIS_COLONNE];

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
bool tousLesEnnemisElimines(Entity ennemis[NB_ENNEMIS_LIGNE][NB_ENNEMIS_COLONNE]) {
    for (int i = 0; i < NB_ENNEMIS_LIGNE; i++) {
        for (int j = 0; j < NB_ENNEMIS_COLONNE; j++) {
            if (ennemis[i][j].actif) {
                return false;
            }
        }
    }
    return true;
}

// Fonction principale du jeu
void gameLoop(SDL_Surface* screen) {
    bool running = true;
    SDL_Event event;

    // Chargement des images
    SDL_Surface* fond = chargerImage("Decor.png");
    SDL_Surface* vaisseau = chargerImage("Joueur.png");
    SDL_Surface* ennemiSurface = chargerImage("Ennemi2.png");
    SDL_Surface* bossSurface = chargerImage("Ennemi1.png");
    SDL_Surface* tirSurface = chargerImage("tir.png");
    SDL_Surface* tirEnnemiSurface = chargerImage("tirennemi.png");

    if (!fond || !vaisseau || !ennemiSurface || !bossSurface || !tirSurface || !tirEnnemiSurface) {
        printf("Erreur : Une ou plusieurs images n'ont pas pu être chargées.\n");
        if (fond) SDL_FreeSurface(fond);
        if (vaisseau) SDL_FreeSurface(vaisseau);
        if (ennemiSurface) SDL_FreeSurface(ennemiSurface);
        if (bossSurface) SDL_FreeSurface(bossSurface);
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
        SDL_FreeSurface(bossSurface);
        SDL_FreeSurface(tirSurface);
        SDL_FreeSurface(tirEnnemiSurface);
        return;
    }
    Mix_VolumeChunk(laser_fx, MIX_MAX_VOLUME / 2);

    // Position initiale du vaisseau
    Entity joueur = {LARGEUR / 2 - LARGEUR_VAISSEAU / 2, HAUTEUR - HAUTEUR_VAISSEAU - 10, true, true, vaisseau};

    // Initialisation des ennemis
    
    for (int i = 0; i < NB_ENNEMIS_LIGNE; i++) {
        for (int j = 0; j < NB_ENNEMIS_COLONNE; j++) {
            ennemis[i][j].x = i * (LARGEUR_ENNEMI + ESPACEMENT_ENNEMI) + 100;
            ennemis[i][j].y = j * (HAUTEUR_ENNEMI + ESPACEMENT_ENNEMI) + 50;
            ennemis[i][j].actif = true;
            ennemis[i][j].directionDroite = true; // Chaque ennemi commence en se déplaçant vers la droite
            ennemis[i][j].surface = ennemiSurface;
        }
    }

    // Initialisation du boss (inactif au départ)
    Entity boss = {LARGEUR / 2 - LARGEUR_ENNEMI / 2, -HAUTEUR_ENNEMI, false, true, bossSurface};

    // Initialisation du tir du joueur
    Entity tir = {-100, -100, false, true, tirSurface};

    // Initialisation des tirs ennemis
    Entity tirsEnnemis[NB_ENNEMIS_LIGNE];
    for (int i = 0; i < NB_ENNEMIS_LIGNE; i++) {
        tirsEnnemis[i].x = -100;
        tirsEnnemis[i].y = -100;
        tirsEnnemis[i].actif = false;
        tirsEnnemis[i].surface = tirEnnemiSurface;
    }

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
                        score += 10;
                        tir.actif = false;

                        if (missileAmeliore) {
                            if (i > 0 && ennemis[i-1][j].actif) { // Ennemi à gauche
                                ennemis[i-1][j].actif = false;
                                score += 10;
                            }
                            if (i < NB_ENNEMIS_LIGNE - 1 && ennemis[i+1][j].actif) { // Ennemi à droite
                                ennemis[i+1][j].actif = false;
                                score += 10;
                            }
                        }
                    }

                }
            }
        }

        // Vérification si tous les ennemis sont morts
        bool tousMorts = true;
        for (int i = 0; i < NB_ENNEMIS_LIGNE; i++) {
            for (int j = 0; j < NB_ENNEMIS_COLONNE; j++) {
                if (ennemis[i][j].actif) {
                    tousMorts = false;
                    break;
                }
            }
        }

       if (tousMorts) {
            if (vagueActuelle < totalVagues) {
                // Afficher l'écran de sélection avant de lancer la vague suivante
                int choix = amelioration(screen, true);

                if (choix == 0) {
                    printf("Pouvoir sélectionné : Bouclier\n");
                    bouclier = 2; // Active un bouclier capable d'encaisser 2 tirs ennemis
                } else if (choix == 1) {
                    printf("Pouvoir sélectionné : Missile puissant\n");
                    missileAmeliore = true; // Active l'effet du missile amélioré
                }


                // Passer à la vague suivante
                vagueActuelle++;
                printf("Début de la vague %d !\n", vagueActuelle);

                // Réinitialiser les ennemis pour la nouvelle vague
                for (int i = 0; i < NB_ENNEMIS_LIGNE; i++) {
                    for (int j = 0; j < NB_ENNEMIS_COLONNE; j++) {
                        ennemis[i][j].x = i * (LARGEUR_ENNEMI + ESPACEMENT_ENNEMI) + 100;
                        ennemis[i][j].y = j * (HAUTEUR_ENNEMI + ESPACEMENT_ENNEMI) + 50;
                        ennemis[i][j].actif = true;
                    }
                }
            } else {
                // Après la 5e vague, activer le boss
                boss.actif = true;
                boss.y = 0;
                printf("Le boss est arrivé !\n");
            }
        }




        // Déplacement du boss
        if (boss.actif) {
            boss.x += boss.directionDroite ? VITESSE_ENNEMI : -VITESSE_ENNEMI;
            if (boss.x + LARGEUR_ENNEMI >= LARGEUR || boss.x <= 0) {
                boss.directionDroite = !boss.directionDroite;
            }
        }

        // Déplacement des ennemis individuels
        for (int i = 0; i < NB_ENNEMIS_LIGNE; i++) {
            for (int j = 0; j < NB_ENNEMIS_COLONNE; j++) {
                if (ennemis[i][j].actif) {
                    ennemis[i][j].x += ennemis[i][j].directionDroite ? VITESSE_ENNEMI : -VITESSE_ENNEMI;

                    // Inverser la direction lorsqu'un ennemi touche un bord
                    if (ennemis[i][j].x + LARGEUR_ENNEMI >= LARGEUR || ennemis[i][j].x <= 0) {
                        ennemis[i][j].directionDroite = !ennemis[i][j].directionDroite;
                        ennemis[i][j].y += VITESSE_DESCENTE; // Descente après toucher un bord
                    }
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

                    if (bouclier > 0) {
                        bouclier--; // Le bouclier absorbe un tir
                        printf("Bouclier impacté ! Restant : %d\n", bouclier);
                    } else {
                        pointDeVie--;
                        if (pointDeVie <= 0) {
                            gameOver = true;
                            running = false;
                        }
                    }
                }

            }
        }

        // Rendu de l'écran
        SDL_FillRect(screen, NULL, SDL_MapRGB(screen->format, 0, 0, 0)); // Noir
        SDL_BlitSurface(fond, NULL, screen, NULL);

        // Rendu du vaisseau
        SDL_Rect rectJoueur = {joueur.x - 80, joueur.y, LARGEUR_VAISSEAU, HAUTEUR_VAISSEAU};
        SDL_BlitSurface(joueur.surface, NULL, screen, &rectJoueur);

        if (bouclier > 0) {
            SDL_Rect rectBouclier = {joueur.x - 10, joueur.y - 10, LARGEUR_VAISSEAU + 20, HAUTEUR_VAISSEAU + 20};
            SDL_FillRect(screen, &rectBouclier, SDL_MapRGB(screen->format, 0, 0, 255)); // Bleu pour le bouclier
        }

        for (int i = 0; i < NB_ENNEMIS_LIGNE; i++) {
            for (int j = 0; j < NB_ENNEMIS_COLONNE; j++) {
                if (ennemis[i][j].actif) {
                    SDL_Rect rectEnnemi = {ennemis[i][j].x - 60, ennemis[i][j].y, LARGEUR_ENNEMI, HAUTEUR_ENNEMI};
                    SDL_BlitSurface(ennemis[i][j].surface, NULL, screen, &rectEnnemi);
                }
            }
        }

        // Rendu du boss
        if (boss.actif) {
            SDL_Rect rectBoss = {boss.x - 60, boss.y, LARGEUR_ENNEMI, HAUTEUR_ENNEMI};
            SDL_BlitSurface(boss.surface, NULL, screen, &rectBoss);
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
    SDL_FreeSurface(bossSurface);
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

    // Afficher l'écran d'accueil et lancer le jeu si l'utilisateur clique sur "PLAY"
    while (1) {
        int choix = accueil(screen);

        if (choix == 1) {
            gameLoop(screen); // Lancer le jeu
        } else if (choix == 2) {
            customisation(screen); // Aller au menu de customisation
        } else if (choix == 3) {
            options(screen); // Aller au menu des options
        } else {
            printf("Fermeture du jeu.\n");
            break; // Quitter la boucle et fermer le programme
        }
    }

    // Vérifier si tous les ennemis sont éliminés et afficher la sélection d'amélioration
    if (tousLesEnnemisElimines(ennemis)) {  
        int choix = amelioration(screen, true);
        if (choix == 0) {
            printf("Amélioration sélectionnée : Bouclier\n");
            // Ajouter le code pour activer le bouclier ici
        } else if (choix == 1) {
            printf("Amélioration sélectionnée : Missile puissant\n");
            // Ajouter le code pour activer les missiles améliorés ici
        }
    }

    SDL_Quit();
    Mix_CloseAudio();
    IMG_Quit();

    return 0;
}


