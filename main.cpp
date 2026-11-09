#include <SDL.h>
#include <cstdlib>
#include <ctime>
#include <vector>
#include <string>

const int CELL = 20;
const int COLS = 32;
const int ROWS = 24;
const int WIDTH = COLS * CELL;
const int HEIGHT = ROWS * CELL;

struct Point {
    int x, y;
};

std::vector<Point> snake;
Point fruit;
int dx = 1, dy = 0;
int score = 0;
bool gameOver = false;

void spawnFruit() {
    fruit = {rand() % COLS, rand() % ROWS};
}

bool occupied(int x, int y) {
    for (auto p : snake)
        if (p.x == x && p.y == y) return true;
    return false;
}

void resetGame() {
    snake.clear();
    snake.push_back({COLS / 2, ROWS / 2});
    snake.push_back({COLS / 2 - 1, ROWS / 2});
    snake.push_back({COLS / 2 - 2, ROWS / 2});

    dx = 1;
    dy = 0;
    score = 0;
    gameOver = false;
    spawnFruit();
}

void draw(SDL_Renderer* renderer, int x, int y, int w, int h,
          Uint8 r, Uint8 g, Uint8 b) {
    SDL_Rect rect = {x, y, w, h};
    SDL_SetRenderDrawColor(renderer, r, g, b, 255);
    SDL_RenderFillRect(renderer, &rect);
}

int main(int argc, char* argv[]) {
    srand((unsigned)time(nullptr));

    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_JOYSTICK) != 0)
        return 1;

    SDL_Window* window = SDL_CreateWindow(
        "Snake PS4",
        SDL_WINDOWPOS_CENTERED,
        SDL_WINDOWPOS_CENTERED,
        WIDTH, HEIGHT,
        SDL_WINDOW_SHOWN
    );

    if (!window) {
        SDL_Quit();
        return 1;
    }

    SDL_Renderer* renderer =
        SDL_CreateRenderer(window, -1, SDL_RENDERER_SOFTWARE);

    if (!renderer) {
        SDL_DestroyWindow(window);
        SDL_Quit();
        return 1;
    }

    SDL_Joystick* controller = nullptr;

    if (SDL_NumJoysticks() > 0)
        controller = SDL_JoystickOpen(0);

    resetGame();

    Uint32 lastMove = SDL_GetTicks();
    bool running = true;

    while (running) {
        SDL_Event event;

        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT)
                running = false;

            if (event.type == SDL_KEYDOWN) {
                switch (event.key.keysym.sym) {
                    case SDLK_UP:
                        if (dy != 1) { dx = 0; dy = -1; }
                        break;
                    case SDLK_DOWN:
                        if (dy != -1) { dx = 0; dy = 1; }
                        break;
                    case SDLK_LEFT:
                        if (dx != 1) { dx = -1; dy = 0; }
                        break;
                    case SDLK_RIGHT:
                        if (dx != -1) { dx = 1; dy = 0; }
                        break;
                    case SDLK_RETURN:
                        if (gameOver) resetGame();
                        break;
                }
            }

            if (event.type == SDL_JOYAXISMOTION && controller) {
                Sint16 value = event.jaxis.value;

                if (event.jaxis.axis == 0 && value < -12000 && dx != 1) {
                    dx = -1; dy = 0;
                }
                if (event.jaxis.axis == 0 && value > 12000 && dx != -1) {
                    dx = 1; dy = 0;
                }
                if (event.jaxis.axis == 1 && value < -12000 && dy != 1) {
                    dx = 0; dy = -1;
                }
                if (event.jaxis.axis == 1 && value > 12000 && dy != -1) {
                    dx = 0; dy = 1;
                }
            }

            if (event.type == SDL_JOYBUTTONDOWN &&
                gameOver && event.jbutton.button == 0) {
                resetGame();
            }
        }

        Uint32 now = SDL_GetTicks();

        if (!gameOver && now - lastMove >= 130) {
            lastMove = now;

            Point head = {
                snake[0].x + dx,
                snake[0].y + dy
            };

            if (head.x < 0 || head.x >= COLS ||
                head.y < 0 || head.y >= ROWS) {
                gameOver = true;
            } else {
                bool eating = head.x == fruit.x && head.y == fruit.y;

                for (size_t i = 0; i < snake.size(); i++) {
                    if (snake[i].x == head.x &&
                        snake[i].y == head.y &&
                        !(i == snake.size() - 1 && !eating)) {
                        gameOver = true;
                        break;
                    }
                }

                if (!gameOver) {
                    snake.insert(snake.begin(), head);

                    if (eating) {
                        score += 10;
                        spawnFruit();
                    } else {
                        snake.pop_back();
                    }
                }
            }
        }

        draw(renderer, 0, 0, WIDTH, HEIGHT, 145, 175, 95);

        for (int x = 0; x < WIDTH; x += CELL)
            for (int y = 0; y < HEIGHT; y += CELL)
                draw(renderer, x, y, CELL - 1, CELL - 1,
                     155, 185, 105);

        draw(renderer, fruit.x * CELL, fruit.y * CELL,
             CELL - 2, CELL - 2, 190, 35, 35);

        for (size_t i = 0; i < snake.size(); i++) {
            if (i == 0)
                draw(renderer, snake[i].x * CELL,
                     snake[i].y * CELL, CELL - 2, CELL - 2,
                     20, 60, 20);
            else
                draw(renderer, snake[i].x * CELL,
                     snake[i].y * CELL, CELL - 2, CELL - 2,
                     30, 90, 35);
        }

        SDL_RenderPresent(renderer);
        SDL_Delay(1);
    }

    if (controller)
        SDL_JoystickClose(controller);

    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();

    return 0;
}
