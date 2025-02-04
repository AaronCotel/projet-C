#include <SDL/SDL.h>
#include <SDL/SDL_ttf.h>
#include <stdbool.h>
#include <stdio.h>

#define LARGEUR 800
#define HAUTEUR 600
#define ESPACE_BOUTON 30

int accueil(SDL_Surface* screen) {
    if (TTF_Init() == -1) {
        printf("Erreur SDL_ttf : %s\n", TTF_GetError());
        return 0;
    }
    
    TTF_Font* font = TTF_OpenFont("C:/Windows/Fonts/arial.ttf", 48);
    if (!font) {
        printf("Erreur chargement de la police : %s\n", TTF_GetError());
        TTF_Quit();
        return 0;
    }
    
    SDL_Color blanc = {255, 255, 255};
    SDL_Color bleu = {100, 100, 255};
    SDL_Color rouge = {255, 0, 0};
    
    SDL_Surface* title = TTF_RenderText_Solid(font, "SPACE INVADER", blanc);
    SDL_Surface* playText = TTF_RenderText_Solid(font, "PLAY", blanc);
    SDL_Surface* customText = TTF_RenderText_Solid(font, "CUSTOMISATION", blanc);
    SDL_Surface* optionsText = TTF_RenderText_Solid(font, "OPTIONS", blanc);
    SDL_Surface* quitText = TTF_RenderText_Solid(font, "QUIT", blanc);

    SDL_Rect titlePos = {LARGEUR / 2 - title->w / 2, 100};
    
    SDL_Rect playButtonPos = {LARGEUR / 2 - playText->w / 2, 250};
    SDL_Rect customButtonPos = {LARGEUR / 2 - customText->w / 2, playButtonPos.y + playText->h + ESPACE_BOUTON};
    SDL_Rect optionsButtonPos = {LARGEUR / 2 - optionsText->w / 2, customButtonPos.y + customText->h + ESPACE_BOUTON};
    SDL_Rect quitButtonPos = {LARGEUR / 2 - quitText->w / 2, optionsButtonPos.y + optionsText->h + ESPACE_BOUTON};
    
    SDL_Rect playButtonRect = {playButtonPos.x - 20, playButtonPos.y - 10, playText->w + 40, playText->h + 20};
    SDL_Rect customButtonRect = {customButtonPos.x - 20, customButtonPos.y - 10, customText->w + 40, customText->h + 20};
    SDL_Rect optionsButtonRect = {optionsButtonPos.x - 20, optionsButtonPos.y - 10, optionsText->w + 40, optionsText->h + 20};
    SDL_Rect quitButtonRect = {quitButtonPos.x - 20, quitButtonPos.y - 10, quitText->w + 40, quitText->h + 20};
    
    bool running = true;
    SDL_Event event;
    int mouseX, mouseY;
    
    while (running) {
        SDL_FillRect(screen, NULL, SDL_MapRGB(screen->format, 0, 0, 0));
        SDL_BlitSurface(title, NULL, screen, &titlePos);
        
        SDL_GetMouseState(&mouseX, &mouseY);
        
        SDL_Rect buttons[] = {playButtonRect, customButtonRect, optionsButtonRect, quitButtonRect};
        SDL_Surface* texts[] = {playText, customText, optionsText, quitText};
        SDL_Rect positions[] = {playButtonPos, customButtonPos, optionsButtonPos, quitButtonPos};
        
        for (int i = 0; i < 4; i++) {
            SDL_Color color = (mouseX >= buttons[i].x && mouseX <= buttons[i].x + buttons[i].w &&
                               mouseY >= buttons[i].y && mouseY <= buttons[i].y + buttons[i].h) ? rouge : bleu;
            SDL_FillRect(screen, &buttons[i], SDL_MapRGB(screen->format, color.r, color.g, color.b));
            SDL_BlitSurface(texts[i], NULL, screen, &positions[i]);
        }
        
        SDL_Flip(screen);
        
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT) {
                running = false;
                return 0;
            }
            if (event.type == SDL_MOUSEBUTTONDOWN) {
                if (mouseX >= playButtonRect.x && mouseX <= playButtonRect.x + playButtonRect.w &&
                    mouseY >= playButtonRect.y && mouseY <= playButtonRect.y + playButtonRect.h) {
                    return 1;
                }
                if (mouseX >= customButtonRect.x && mouseX <= customButtonRect.x + customButtonRect.w &&
                    mouseY >= customButtonRect.y && mouseY <= customButtonRect.y + customButtonRect.h) {
                    return 2;
                }
                if (mouseX >= optionsButtonRect.x && mouseX <= optionsButtonRect.x + optionsButtonRect.w &&
                    mouseY >= optionsButtonRect.y && mouseY <= optionsButtonRect.y + optionsButtonRect.h) {
                    return 3;
                }
                if (mouseX >= quitButtonRect.x && mouseX <= quitButtonRect.x + quitButtonRect.w &&
                    mouseY >= quitButtonRect.y && mouseY <= quitButtonRect.y + quitButtonRect.h) {
                    return 0;
                }
            }
        }
        SDL_Delay(16);
    }
    
    SDL_FreeSurface(title);
    SDL_FreeSurface(playText);
    SDL_FreeSurface(customText);
    SDL_FreeSurface(optionsText);
    SDL_FreeSurface(quitText);
    TTF_CloseFont(font);
    TTF_Quit();
    return 0;
}
