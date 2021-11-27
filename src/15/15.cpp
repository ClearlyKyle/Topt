
#include "topt.h"

#include <vector>

#include "SDL2/SDL2_gfxPrimitives.h"

#define EMPTY 0
#define PLAYER_X 1
#define PLAYER_O 2

#define RUNNING_STATE 0
#define START_TIMER 1
#define QUIT_STATE 2
#define RANDOMISE_BOARD 3
#define WON_STATE 4
#define DISPLAY_TIME_SCREEN 5
#define WAIT_FOR_CLICK 6

#define N 4 // number of rows and columns 4x4

// make the screen square
#define SCREEN_WIDTH 320
#define SCREEN_HEIGHT SCREEN_WIDTH

const int CELL_WIDTH = SCREEN_WIDTH / N;
const int CELL_HEIGHT = SCREEN_HEIGHT / N;

const SDL_Color GRID_COLOUR = {.r = 255, .g = 255, .b = 255};
const SDL_Color TEXT_COLOUR_WHITE = {255, 255, 255};

// pixel offset for inner square around the numbers
const int cell_offset = 3;
const SDL_Rect number_square = {.x = cell_offset, .y = cell_offset, .w = CELL_WIDTH - cell_offset, .h = CELL_HEIGHT - cell_offset};

class FifteenPuzzle : public Topt::Rasterizer
{
private:
    typedef struct
    {
        int board[N * N];
        int state;
    } game_t;

    // Handle Timing
    std::chrono::steady_clock::time_point m_Time_Begin;
    std::chrono::steady_clock::time_point m_Time_End;

    game_t m_game;

public:
    virtual bool OnUserCreate()
    {
        m_game = {
            .board = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16},
            .state = START_TIMER, // to trigger the randomizer
        };

        return true;
    }

    virtual bool OnUserUpdate(float fElapsedTime)
    {
        if (mouse.SingleClick())
        {
            Click_on_cell(mouse.GetX() / CELL_WIDTH, mouse.GetY() / CELL_HEIGHT);
        }
        Render_game();
        return true;
    }

private:
    void Render_grid(const SDL_Color *colour)
    {
        SetRenderDrawColour(*colour);

        for (int i = 0; i < N; i++)
        {
            for (int j = 0; j < N; j++)
            {
                SDL_Rect tmp = {
                    .x = number_square.x + (i * CELL_WIDTH),
                    .y = number_square.y + (j * CELL_HEIGHT),
                    .w = number_square.w,
                    .h = number_square.h,
                };
                DrawRect(&tmp);
            }
        }
    }

    void Render_number(int number, int row, int column)
    {
        // our blank space number
        if (number == 16)
            return;

        // this opens a font style and sets a size
        TTF_Font *font = TTF_OpenFont("..\\res\\fonts\\RobotoMono-Bold.ttf", 32);
        if (!font)
        {
            fprintf(stderr, "Error using TTF_OpenFont: %s\n", TTF_GetError());
            return;
        }

        // as TTF_RenderText_Solid could only be used on
        // SDL_Surface then you have to create the surface first
        char char_number[2];
        sprintf(char_number, "%d", number);
        SDL_Surface *surfaceMessage = TTF_RenderText_Blended(font, char_number, TEXT_COLOUR_WHITE);
        if (!surfaceMessage)
        {
            fprintf(stderr, "Error using TTF_RenderText_Blended: %s\n", TTF_GetError());
            return;
        }
        // now you can convert it into a texture
        SDL_Texture *Message = SDL_CreateTextureFromSurface(GetRenderer(), surfaceMessage);

        // const float half_box_side = fmin(CELL_WIDTH, CELL_HEIGHT) * 0.25;
        SDL_Rect Message_rect; // create a rect
        const int half_textbox_width = surfaceMessage->w * 0.5;
        const int half_textbox_hight = (surfaceMessage->h * 0.5) - 1;
        Message_rect.x = column * CELL_WIDTH + (CELL_WIDTH * 0.5 - half_textbox_width); // controls the rect's x coordinate
        Message_rect.y = row * CELL_HEIGHT + (CELL_HEIGHT * 0.5 - half_textbox_hight);  // controls the rect's y coordinte
        Message_rect.w = surfaceMessage->w;                                             // controls the width of the rect
        Message_rect.h = surfaceMessage->h;                                             // controls the height of the rect

        SDL_RenderCopy(GetRenderer(), Message, NULL, &Message_rect);

        // Don't forget to free your surface and texture
        TTF_CloseFont(font);
        SDL_FreeSurface(surfaceMessage);
        SDL_DestroyTexture(Message);
    }

    void Render_board()
    {
        /* board array is
            1   2  3  4
            5   6  7  8
            9  10 11 12
            13 14 15 16
        */
        Render_grid(&GRID_COLOUR);
        for (int i = 0; i < N; ++i)
        {
            for (int j = 0; j < N; ++j)
            {
                Render_number(m_game.board[i * N + j], i, j);
            }
        }
    }

    void Render_game()
    {
        switch (m_game.state)
        {
        case RANDOMISE_BOARD:
            Randomize_board();
            // break;
        case START_TIMER:
            std::cout << "Timer started...\n";
            m_Time_Begin = std::chrono::steady_clock::now();
            m_game.state = RUNNING_STATE;
            //  break;
        case RUNNING_STATE:
            Render_board();
            break;
        case DISPLAY_TIME_SCREEN:
            Display_game_time();
            break;
        default:
            break;
        }
    }

    void Game_over_condition()
    {
        for (int i = 0; i < 16; i++)
        {
            if (m_game.board[i] != (i + 1))
            {
                return;
            }
        }

        std::cout << "GAME COMPLETE!\n";
        m_game.state = DISPLAY_TIME_SCREEN;

        m_Time_End = std::chrono::steady_clock::now();
    }

    void Move(int from_x, int from_y, int to_x, int to_y)
    {
        m_game.board[from_y * N + from_x] = m_game.board[to_y * N + to_x];
        m_game.board[to_y * N + to_x] = 16;
        Game_over_condition();
    }

    // row and column are indicies of where we clicked
    void Move_block(int row, int column)
    {
        // MOVE RIGHT
        if (row < 3)
        {
            if (m_game.board[column * N + (row + 1)] == 16)
            {
                m_game.board[column * N + (row + 1)] = m_game.board[column * N + row];
                m_game.board[column * N + row] = 16;
                Game_over_condition();

                // Move(game, column, row + 1, row, column);
                return;
            }
        }
        // MOVE LEFT
        if (row > 0)
        {
            if (m_game.board[column * N + (row - 1)] == 16)
            {
                m_game.board[column * N + (row - 1)] = m_game.board[column * N + row];
                m_game.board[column * N + row] = 16;
                Game_over_condition();
                return;
            }
        }
        // MOVE UP
        if (column < 3)
        {
            if (m_game.board[(column + 1) * N + row] == 16)
            {
                m_game.board[(column + 1) * N + row] = m_game.board[column * N + row];
                m_game.board[column * N + row] = 16;
                Game_over_condition();
                return;
            }
        }
        // MOVE DOWN
        if (column > 0)
        {
            if (m_game.board[(column - 1) * N + row] == 16)
            {
                m_game.board[(column - 1) * N + row] = m_game.board[column * N + row];
                m_game.board[column * N + row] = 16;
                Game_over_condition();
                return;
            }
        }
    }

    void Reset_game()
    {
        m_game.state = RUNNING_STATE;

        for (int i = 0; i < 16; i++)
        {
            m_game.board[i] = i + 1;
        }
    }

    void Randomize_board()
    {
        int x = 3; // starting index for "blank" square
        int y = 3;

        std::cout << "Starting to randomize the board!\n";
        for (int i = 0; i < 750; i++)
        {
            x += (rand() % 3) - 1;
            x = x > 3 ? 3 : x;
            x = x < 0 ? 0 : x;

            y += (rand() % 3) - 1;
            y = y > 3 ? 3 : y;
            y = y < 0 ? 0 : y;

            Move_block(x, y);
        }
        std::cout << "Finsihed randomizing the board!\n";
        m_game.state = RUNNING_STATE;
    }

    void Display_game_time()
    {
        TTF_Font *font = TTF_OpenFont("..\\res\\fonts\\RobotoMono-Bold.ttf", 32);
        if (!font)
        {
            fprintf(stderr, "Error using TTF_OpenFont: %s\n", TTF_GetError());
            return;
        }

        // https://www.delftstack.com/howto/cpp/cpp-timing/
        // divide by 1000 so we can get like 13.37 seconds :)
        auto end_time = (std::chrono::duration_cast<std::chrono::milliseconds>(m_Time_End - m_Time_Begin).count()) / 1000.0f;

        char char_time[4];
        sprintf(char_time, "%0.1f", end_time);

        SDL_Surface *surface_time = TTF_RenderText_Blended(font, char_time, TEXT_COLOUR_WHITE);
        SDL_Surface *surface_letter_s = TTF_RenderText_Blended(font, "s", TEXT_COLOUR_WHITE);
        if (!surface_time || !surface_letter_s)
        {
            fprintf(stderr, "Error using TTF_RenderText_Blended: %s\n", TTF_GetError());
            return;
        }
        // now you can convert it into a texture
        SDL_Texture *texture_time = SDL_CreateTextureFromSurface(GetRenderer(), surface_time);
        SDL_Texture *texture_letter_s = SDL_CreateTextureFromSurface(GetRenderer(), surface_letter_s);

        // center text on screen
        const int x_pos = (SCREEN_WIDTH / 2) - ((surface_time->w + surface_letter_s->w) / 2);
        const int y_pos = (SCREEN_HEIGHT / 2) - ((surface_time->h) / 2);
        SDL_Rect time_rect_dest;            // create a rect
        time_rect_dest.x = x_pos;           // controls the rect's x coordinate
        time_rect_dest.y = y_pos;           // controls the rect's y coordinte
        time_rect_dest.w = surface_time->w; // controls the width of the rect
        time_rect_dest.h = surface_time->h; // controls the height of the rect

        SDL_Rect letter_s_dest = {.x = (time_rect_dest.x + time_rect_dest.w),
                                  .y = y_pos,
                                  .w = surface_letter_s->w,
                                  .h = surface_letter_s->h};

        SDL_RenderCopy(GetRenderer(), texture_time, NULL, &time_rect_dest);

        SDL_RenderCopy(GetRenderer(), texture_letter_s, NULL, &letter_s_dest);

        // Don't forget to free the surface and texture
        TTF_CloseFont(font);
        SDL_FreeSurface(surface_time);
        SDL_DestroyTexture(texture_time);

        SDL_FreeSurface(surface_letter_s);
        SDL_DestroyTexture(texture_letter_s);
    }

    // what to do when user clicks the screen
    void Click_on_cell(int row, int column)
    {
        if (m_game.state == RUNNING_STATE)
        {
            Move_block(row, column);
        }
        else if (m_game.state == WON_STATE)
        {
            std::cout << "The game has been complete!\n";
            m_game.state = DISPLAY_TIME_SCREEN;
        }
        else if (m_game.state == DISPLAY_TIME_SCREEN)
        {
            std::cout << "Randomising the board again!\n";
            m_game.state = RANDOMISE_BOARD;
        }
        else // press reset key
        {
            Reset_game();
        }
    }
};

int main(int argc, char *argv[])
{
    FifteenPuzzle puzzle;

    puzzle.Init(SCREEN_WIDTH, SCREEN_HEIGHT);
    puzzle.Start();

    return 1;
}