#include "Connect4/connect4.h"

bool Connect4_Init_Dependencies() {
    if (!SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO)) {
        SDL_Log("SDL_Init failed: %s", SDL_GetError());
        return false;
    }
    if (!TTF_Init()) {
        SDL_Log("TTF_Init failed: %s", SDL_GetError());
        return false;
    }
    return true;
}

void Connect4_Quit_Dependencies() {
    TTF_Quit();
    SDL_Quit();
}

Connect4* Connect4_create(uint8_t boardWidth, uint8_t boardHeight) {
    Connect4* connect4 = calloc(1, sizeof(Connect4));
    if (!connect4) {
        // Out of memory
        return NULL;
    }
    connect4->window = SDL_CreateWindow("Connect4", 1280, 720, 0);
    if (!connect4->window) {
        Connect4_destroy(connect4);
        return NULL;
    }
    connect4->renderer = SDL_CreateRenderer(connect4->window, NULL);
    if (!connect4->renderer) {
        Connect4_destroy(connect4);
        return NULL;
    }
    connect4->font_regular = TTF_OpenFont("./assets/fonts/Monocraft.ttf", 32);
    if (!connect4->font_regular) {
        Connect4_destroy(connect4);
        return NULL;
    }
    connect4->font_bold = TTF_OpenFont("./assets/fonts/Monocraft-Bold.ttf", 32);
    if (!connect4->font_bold) {
        Connect4_destroy(connect4);
        return NULL;
    }
    connect4->board = Board_create(boardWidth, boardHeight);
    return connect4;
}

void Connect4_destroy(Connect4* connect4) {
    if (!connect4) {
        return;
    }
    if (connect4->font_regular) {
        TTF_CloseFont(connect4->font_regular);
    }
    if (connect4->font_bold) {
        TTF_CloseFont(connect4->font_bold);
    }
    if (connect4->renderer) {
        SDL_DestroyRenderer(connect4->renderer);
    }
    if (connect4->window) {
        SDL_DestroyWindow(connect4->window);
    }
    free(connect4);
}

void Connect4_run(Connect4* connect4) {
    Board_set_slot(&connect4->board, 0, connect4->board.height - 1, Player1);
    bool running = true;
    SDL_Event event;
    while (running) {
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_EVENT_QUIT) {
                running = false;
            }
        }
        Board_update_test_string(&connect4->board, connect4->testStr);
        SDL_SetRenderDrawColor(connect4->renderer, 0, 0, 0, 255);
        SDL_RenderClear(connect4->renderer);
        Connect4_drawTestStr(connect4);
        SDL_RenderPresent(connect4->renderer);
    }
}

void Connect4_drawTestStr(Connect4* connect4) {
    SDL_Color white = {255, 255, 255, 255};
    SDL_Surface* textSurface = TTF_RenderText_Blended_Wrapped(connect4->font_regular, connect4->testStr, 0, white, 800);  
    if (textSurface) {
        SDL_Texture* textTexture = SDL_CreateTextureFromSurface(connect4->renderer, textSurface);
        SDL_FRect destRect = {50, 50, (float)textSurface->w, (float)textSurface->h};
        SDL_RenderTexture(connect4->renderer, textTexture, NULL, &destRect);
        SDL_DestroyTexture(textTexture);
        SDL_DestroySurface(textSurface);
    }
}