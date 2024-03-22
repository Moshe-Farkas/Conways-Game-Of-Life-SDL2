#include <SDL2/SDL.h>
#include <stdbool.h>
#include <stdlib.h>
#include <time.h>

#define SCREEN_WIDTH 1500
#define SCREEN_HEIGHT 1000
#define BLOCK_SIZE (SCREEN_WIDTH / 300)
#define BOARD_WIDTH (SCREEN_WIDTH / BLOCK_SIZE)
#define BOARD_HEIGHT (SCREEN_HEIGHT / BLOCK_SIZE)

enum State {
    DRAWING_STATE,
    RUN_STATE,
};

enum State gState;
bool gBoard[BOARD_HEIGHT][BOARD_WIDTH];
bool gLastBoard[BOARD_HEIGHT][BOARD_WIDTH];

int neighborCount(int i, int j) {
    int nCount = 0;
    if (i - 1 >= 0 && j - 1 >= 0 && gBoard[i - 1][j - 1] == true) {
        // up-left
        nCount++;
    } 
    if (i - 1 >= 0 && gBoard[i - 1][j] == true) {
        // up
        nCount++;
    } 
    if (i - 1 >= 0 && j + 1 < BOARD_WIDTH && gBoard[i - 1][j + 1] == true) {
        // up-right
        nCount++;
    }
    if (j + 1 < BOARD_WIDTH && gBoard[i][j + 1]) {
        // right
        nCount++;
    } 
    if (i + 1 < BOARD_HEIGHT && j + 1 < BOARD_WIDTH && gBoard[i + 1][j + 1] == true) {
        // down-right
        nCount++;
    }
    if (i + 1 < BOARD_HEIGHT && gBoard[i + 1][j] == true) {
        // down
        nCount++;
    } 
    if (i + 1 < BOARD_HEIGHT && j - 1 >= 0 && gBoard[i + 1][j - 1] == true) {
        // down-left
        nCount++;
    }
    if (j - 1 >= 0 && gBoard[i][j - 1] == true) {
        // left
        nCount++;
    }
    return nCount;
}

void copyBoard(bool src[BOARD_HEIGHT][BOARD_WIDTH], bool dst[BOARD_HEIGHT][BOARD_WIDTH]) {
    for (int i = 0; i < BOARD_HEIGHT; i++) {
        for (int j = 0; j < BOARD_WIDTH; j++) {
            dst[i][j] = src[i][j];
        }
    }
}

void update() {
    // Any live cell with fewer than two live neighbors dies, as if by underpopulation.
    // Any live cell with two or three live neighbors lives on to the next generation.
    // Any live cell with more than three live neighbors dies, as if by overpopulation.
    // Any dead cell with exactly three live neighbors becomes a live cell, as if by reproduction.
    bool updates[BOARD_HEIGHT][BOARD_WIDTH];
    for (int i = 0; i < BOARD_HEIGHT; i++) {
        for (int j = 0; j < BOARD_WIDTH; j++) {
            updates[i][j] = gBoard[i][j];
            int nCount = neighborCount(i, j);
            if (gBoard[i][j] == true) {
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
    copyBoard(updates, gBoard);
}

SDL_Point mouseToBoardCoordinates(int x, int y) {
    SDL_Point p = {x / BLOCK_SIZE, y / BLOCK_SIZE};
    return p;
}

void clearBoard() {
    for (int i = 0; i < BOARD_HEIGHT; i++) {
        for (int j = 0; j < BOARD_WIDTH; j++) {
            gBoard[i][j] = false;
        }
    }
}

void enterRunState() {
    gState = RUN_STATE;
    // save last board state
    copyBoard(gBoard, gLastBoard);
}

void updateDrawState(SDL_Event e) {
    if (e.type == SDL_MOUSEBUTTONDOWN) {
        int mouseX = e.motion.x;
        int mouseY = e.motion.y;
        SDL_Point boardCorrdinates = mouseToBoardCoordinates(mouseX, mouseY);
        int boardX = boardCorrdinates.x;
        int boardY = boardCorrdinates.y;
        if (e.button.button == SDL_BUTTON_LEFT) {
            gBoard[boardY][boardX] = true;
        } else if (e.button.button == SDL_BUTTON_RIGHT) {
            gBoard[boardY][boardX] = false;
        }
    } 
    if (e.type == SDL_KEYDOWN) {
        switch (e.key.keysym.sym) {
            case SDLK_SPACE: 
                enterRunState();
                break;
            case SDLK_c:
                clearBoard();
                break;
        }
    }
}

void enterDrawingState() {
    gState = DRAWING_STATE;
    // apply last drawing state board to gBoard
    copyBoard(gLastBoard, gBoard);
}

void updateRunState(SDL_Event e) {
    if (e.type == SDL_KEYDOWN && e.key.keysym.sym == SDLK_SPACE) {
        enterDrawingState();
    }
}

void draw(SDL_Renderer* renderer) {
    SDL_SetRenderDrawColor(renderer, 0x00, 0x00, 0x00, 0xff);
    SDL_RenderClear(renderer);
    for (int i = 0; i < BOARD_HEIGHT; i++) {
        for (int j = 0; j < BOARD_WIDTH; j++) {
            if (gBoard[i][j] == true) {
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

void usage() {
    printf("Usage: --mode [draw | random]\n");
}

void fillBoardWithRandom() {
    // rand() % (max_number + 1 - minimum_number) + minimum_number
    for (int i = 0; i < BOARD_HEIGHT; i++) {
        for (int j = 0; j < BOARD_WIDTH; j++) {
            if (rand() % 4 == 0) {
                gBoard[i][j] = true;
            }
        }
    }
}

int main(int argc, char* args[]) {
    // printf("board width: %d\n", BOARD_WIDTH);
    // printf("board height: %d\n", BOARD_HEIGHT);
    if (argc != 3) {
        usage();
        return 0;
    }
    char* mode = args[2];
    if (strcmp(mode, "random") == 0) {
        srand(time(NULL));
        fillBoardWithRandom();
        enterRunState();
    } else if (strcmp(mode, "draw") == 0) {
        // as is
        enterDrawingState();
    }


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

    SDL_Event e; bool quit = false;
    while (!quit) {
        while (SDL_PollEvent(&e) != 0) {
            if (e.type == SDL_QUIT) {
                quit = true;
            } else if (gState == DRAWING_STATE) {
                updateDrawState(e);
            } else if (gState == RUN_STATE) {
                updateRunState(e);
            }
        }
        if (gState == RUN_STATE) {
            update(gBoard);
        }
        draw(renderer);
        SDL_Delay(60);
    }

    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();

    return 0;
}