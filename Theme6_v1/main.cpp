#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <cmath>
#include <string>

const double PI = 3.14159265358979323846;

void drawFilledSector(SDL_Renderer* renderer,
                      int cx, int cy,
                      int rx, int ry,
                      double startAngle,
                      double endAngle,
                      SDL_Color color)
{
    SDL_SetRenderDrawColor(renderer, color.r, color.g, color.b, 255);

    for (double a = startAngle; a <= endAngle; a += 0.01) {
        int x = cx + rx * cos(a);
        int y = cy + ry * sin(a);
        SDL_RenderDrawLine(renderer, cx, cy, x, y);
    }
}

void drawText(SDL_Renderer* renderer, TTF_Font* font,
              const std::string& text, int x, int y)
{
    SDL_Color black = {0, 0, 0, 255};
    SDL_Surface* surface = TTF_RenderUTF8_Blended(font, text.c_str(), black);
    SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, surface);

    SDL_Rect dst = {x - surface->w / 2, y - surface->h / 2,
                    surface->w, surface->h};

    SDL_RenderCopy(renderer, texture, nullptr, &dst);

    SDL_FreeSurface(surface);
    SDL_DestroyTexture(texture);
}

int main(int argc, char* argv[])
{
    SDL_Init(SDL_INIT_VIDEO);
    TTF_Init();

    SDL_Window* window = SDL_CreateWindow(
        "Еліптична діаграма Тема 6, завдання 1",
        SDL_WINDOWPOS_CENTERED,
        SDL_WINDOWPOS_CENTERED,
        800, 600,
        SDL_WINDOW_RESIZABLE
    );

    SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);

    TTF_Font* font = TTF_OpenFont("/usr/share/fonts/truetype/dejavu/DejaVuSans.ttf", 20);

    bool running = true;
    SDL_Event event;

    while (running) {
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT)
                running = false;
        }

        int w, h;
        SDL_GetWindowSize(window, &w, &h);

        int cx = w / 2;
        int cy = h / 2;
        int rx = w / 4;
        int ry = h / 4;

        SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
        SDL_RenderClear(renderer);

        double a1 = 0;
        double a2 = a1 + 2 * PI * 0.25;
        double a3 = a2 + 2 * PI * 0.65;
        double a4 = a3 + 2 * PI * 0.10;

        drawFilledSector(renderer, cx, cy, rx, ry, a1, a2, {255, 0, 0});
        drawFilledSector(renderer, cx, cy, rx, ry, a2, a3, {0, 255, 0});
        drawFilledSector(renderer, cx, cy, rx, ry, a3, a4, {0, 0, 255});

        drawText(renderer, font, "25%", cx + rx * cos((a1 + a2) / 2),
                                   cy + ry * sin((a1 + a2) / 2));

        drawText(renderer, font, "65%", cx + rx * cos((a2 + a3) / 2),
                                   cy + ry * sin((a2 + a3) / 2));

        drawText(renderer, font, "10%", cx + rx * cos((a3 + a4) / 2),
                                   cy + ry * sin((a3 + a4) / 2));

        SDL_RenderPresent(renderer);
    }

    TTF_CloseFont(font);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    TTF_Quit();
    SDL_Quit();

    return 0;
}
