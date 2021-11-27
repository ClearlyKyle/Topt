
#include "topt.h"

#include "SDL2/SDL_ttf.h"
#include "SDL2/SDL2_gfxPrimitives.h"

#define EMPTY 0
#define PLAYER_X 1
#define PLAYER_O 2

#define RUNNING_STATE 0
#define QUIT_STATE 1
#define WON_STATE 2

#define N 4 // number of rows and columns 4x4

// make the screen square
#define SCREEN_WIDTH 640
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
        float time;
    } game_t;

    game_t m_game;

public:
    virtual bool OnUserCreate()
    {
        game_t game = {
            .board = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16},
            .state = WON_STATE, // to trigger the randomizer
            .time = 0.0f};

        return true;
    }

    virtual bool OnUserUpdate(float fElapsedTime)
    {
        if (mouse.SingleClick())
        {
            Click_on_cell(mouse.GetY() / CELL_HEIGHT, mouse.GetX() / CELL_WIDTH);
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
        if (number == 16)
            return;

        // const float center_x = CELL_WIDTH * 0.5 + column * CELL_WIDTH;
        // const float center_y = CELL_HEIGHT * 0.5 + row * CELL_HEIGHT;

        // this opens a font style and sets a size
        TTF_Font *font = TTF_OpenFont("..\\15\\res\\fonts\\RobotoMono-Bold.ttf", 32);
        if (!font)
        {
            fprintf(stderr, "Error using TTF_OpenFont: %s\n", TTF_GetError());
            return;
        }
        // TTF_SetFontStyle(font, TTF_STYLE_BOLD);

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
        // SDL_Surface *surfaceMessage = TTF_RenderGlyph_Blended(font, char_number, White);
        // SDL_Surface *surfaceMessage = TTF_RenderUNICODE_Blended(font, &char_number, White);

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

        // SDL_Rect rectToDraw = {Message_rect.x, Message_rect.y, surfaceMessage->w, surfaceMessage->h};
        //  SDL_RenderDrawRect(renderer, &Message_rect); // draw rectangle with no colour (border only)

        // (0,0) is on the top left of the window/screen,d
        // think a rect as the text's box,
        // that way it would be very simple to understand

        // Now since it's a texture, you have to put RenderCopy
        // in your game loop area, the area where the whole code executes

        // you put the renderer's name first, the Message,
        // the crop size (you can ignore this if you don't want
        // to dabble with cropping), and the rect which is the size
        // and coordinate of your texture
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
        case RUNNING_STATE:
            Render_board();
            break;
        // case WON_STATE:   // display time to solve
        //     Render_won_screen(renderer, game);
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
        m_game.state = WON_STATE;
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

        for (int i = 0; i < 100; i++)
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

    void Click_on_cell(int row, int column)
    {
        if (m_game.state == RUNNING_STATE)
        {
            Move_block(row, column);
        }
        else if (m_game.state == WON_STATE)
        {
            std::cout << "Randomizing the board!\n";
            Randomize_board();
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