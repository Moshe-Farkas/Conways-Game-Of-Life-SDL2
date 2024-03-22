#include <SDL2/SDL.h>
#include <stdbool.h>

#define SCREEN_WIDTH 1000
#define SCREEN_HEIGHT 640
#define BLOCK_SIZE (SCREEN_WIDTH / 60)
#define BOARD_WIDTH (SCREEN_WIDTH / BLOCK_SIZE)
#define BOARD_HEIGHT (SCREEN_HEIGHT / BLOCK_SIZE)

int neighborCount(bool board[BOARD_HEIGHT][BOARD_WIDTH], int i, int j) {
    int nCount = 0;
    if (i - 1 >= 0 && j - 1 >= 0 && board[i - 1][j - 1] == true) {
        // up-left
        nCount++;
    } 
    if (i - 1 >= 0 && board[i - 1][j] == true) {
        // up
        nCount++;
    } 
    if (i - 1 >= 0 && j + 1 < BOARD_WIDTH && board[i - 1][j + 1] == true) {
        // up-right
        nCount++;
    }
    if (j + 1 < BOARD_WIDTH && board[i][j + 1]) {
        // right
        nCount++;
    } 
    if (i + 1 < BOARD_HEIGHT && j + 1 < BOARD_WIDTH && board[i + 1][j + 1] == true) {
        // down-right
        nCount++;
    }
    if (i + 1 < BOARD_HEIGHT && board[i + 1][j] == true) {
        // down
        nCount++;
    } 
    if (i + 1 < BOARD_HEIGHT && j - 1 >= 0 && board[i + 1][j - 1] == true) {
        // down-left
        nCount++;
    }
    if (j - 1 >= 0 && board[i][j - 1] == true) {
        // left
        nCount++;
    }
    return nCount;
}

void update(bool board[BOARD_HEIGHT][BOARD_WIDTH]) {
    // Any live cell with fewer than two live neighbors dies, as if by underpopulation.
    // Any live cell with two or three live neighbors lives on to the next generation.
    // Any live cell with more than three live neighbors dies, as if by overpopulation.
    // Any dead cell with exactly three live neighbors becomes a live cell, as if by reproduction.
    bool updates[BOARD_HEIGHT][BOARD_WIDTH];
    for (int i = 0; i < BOARD_HEIGHT; i++) {
        for (int j = 0; j < BOARD_WIDTH; j++) {
            updates[i][j] = board[i][j];
            int nCount = neighborCount(board, i, j);
            if (board[i][j] == true) {
                // alive
                if (nCount < 2) {
                    updates[i][j] = false;
                } else if (nCount > 3) {
                    updates[i][j] = false;
                }
            } else {
                // dead 
                if (nCount == 3) {
                    updates[i][j] = true;
                }
            }
        }
    }
    // apply update
    for (int i = 0; i < BOARD_HEIGHT; i++) {
        for (int j = 0; j < BOARD_WIDTH; j++) {
            board[i][j] = updates[i][j];
        }
    }
}

void draw(SDL_Renderer* renderer, bool board[BOARD_HEIGHT][BOARD_WIDTH]) {
    SDL_SetRenderDrawColor(renderer, 0x00, 0x00, 0x00, 0xff);
    SDL_RenderClear(renderer);
    for (int i = 0; i < BOARD_HEIGHT; i++) {
        for (int j = 0; j < BOARD_WIDTH; j++) {
            if (board[i][j] == true) {
                int x = j * BLOCK_SIZE;
                int y = i * BLOCK_SIZE;
                SDL_Rect temp = {x, y, BLOCK_SIZE, BLOCK_SIZE};
                SDL_SetRenderDrawColor(renderer, 0xff, 0xff, 0xff, 0xff);
                SDL_RenderFillRect(renderer, &temp);
                SDL_SetRenderDrawColor(renderer, 0x00, 0x00, 0x00, 0xff);
            }
        }
    }
    SDL_RenderPresent(renderer);
}

int main(int argc, char* args[]) {
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        SDL_Log("%s\n", SDL_GetError());
        return 1;
    }
    SDL_Window* window = SDL_CreateWindow(
        "conways",
        SDL_WINDOWPOS_UNDEFINED,
        SDL_WINDOWPOS_UNDEFINED,
        SCREEN_WIDTH,
        SCREEN_HEIGHT,
        0
    );
    if (window == NULL) {
        SDL_Log("%s\n", SDL_GetError());
        return 1;
    }
    SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    if (renderer == NULL) {
        SDL_Log("%s\n", SDL_GetError());
        return 1;
    }
    bool board[BOARD_HEIGHT][BOARD_WIDTH] = {0};
    board[1][2] = true;
    board[2][3] = true;
    board[3][1] = true;
    board[3][2] = true;
    board[3][3] = true;


    SDL_Event e; bool quit = false;
    while (!quit) {
        while (SDL_PollEvent(&e) != 0) {
            // if (e.type == SDL_QUIT) {
            //     quit = true;
            // }
            switch (e.type) {
                case SDL_QUIT:
                    quit = true;
                    break;
                case SDL_MOUSEBUTTONDOWN:
                    printf("here");
                    break;
            }
        }
        update(board);
        draw(renderer, board);
        SDL_Delay(60);
    }

    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();

    return 0;
}