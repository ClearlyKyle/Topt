
#include "topt.h"
#include "SDL2/SDL2_gfxPrimitives.h"

const int board_size = 3;

#define RUNNING_STATE 0
#define X_WON_STATE 1
#define O_WON_STATE 2
#define TIE_STATE 3
#define QUIT_STATE 4
#define DISPLAY_SCORE_STATE 5

#define EMPTY 0
#define PLAYER_X 1
#define PLAYER_O 2

#define N 3 // number of rows and columns 3x3

#define SCREEN_WIDTH 640
#define SCREEN_HEIGHT 480

static const SDL_Color GRID_COLOUR = {.r = 255, .g = 255, .b = 255};
static const SDL_Color PLAYER_X_COLOUR = {.r = 255, .g = 50, .b = 50};
static const SDL_Color PLAYER_O_COLOUR = {.r = 50, .g = 100, .b = 255};
static const SDL_Color TIE_COLOUR = {.r = 100, .g = 100, .b = 100};

static const SDL_Color TEXT_COLOUR_WHITE = {255, 255, 255};

const int cell_width = 213;
const int cell_height = 160;

class TicTacToe : public Topt::Rasterizer
{
private:
    typedef struct
    {
        int board[board_size * board_size];
        int player;
        int state;
    } game_t;

    game_t m_game;

    int x_win_counter;
    int o_win_counter;

public:
    virtual bool OnUserCreate()
    {
        m_game = {
            .board = {EMPTY, EMPTY, EMPTY,
                      EMPTY, EMPTY, EMPTY,
                      EMPTY, EMPTY, EMPTY},
            .player = PLAYER_X,
            .state = RUNNING_STATE};

        x_win_counter = 0;
        o_win_counter = 0;

        return true;
    }

    virtual bool OnUserUpdate(float fElapsedTime)
    {
        if (mouse.SingleClick())
        {
            Click_on_cell(mouse.GetY() / cell_height, mouse.GetX() / cell_width);
        }
        Render_game();
        return true;
    }

private:
    int Check_player_won(int player)
    {
        int row_count = 0;
        int coulmn_count = 0;
        int diag_count = 0;
        int diag2_count = 0;

        for (int i = 0; i < N; i++)
        {
            for (int j = 0; j < N; j++)
            {
                if (m_game.board[i * N + j] == player)
                {
                    row_count++;
                }
                if (m_game.board[j * N + i] == player)
                {
                    coulmn_count++;
                }
            }
            if (row_count >= N || coulmn_count >= N)
            {
                return 1;
            }
            row_count = 0;
            coulmn_count = 0;

            if (m_game.board[i * N + i] == player)
            {
                diag_count++;
            }
            if (m_game.board[i * N + N - i - 1] == player)
            {
                diag2_count++;
            }
        }

        return diag_count >= N || diag2_count >= N;
    }

    int Count_cells(const int *board, int cell)
    {
        int count = 0;
        for (int i = 0; i < N * N; i++)
        {
            if (board[i] == cell)
            {
                count++;
            }
        }

        return count;
    }

    void Game_over_condition()
    {
        if (Check_player_won(PLAYER_X))
        {
            m_game.state = X_WON_STATE;
            x_win_counter++;
        }
        else if (Check_player_won(PLAYER_O))
        {
            m_game.state = O_WON_STATE;
            o_win_counter++;
        }
        else if (Count_cells(m_game.board, EMPTY) == 0)
        {
            m_game.state = TIE_STATE;
        }
    }

    void Player_turn(int row, int column)
    {
        if (m_game.board[row * N + column] == EMPTY)
        {
            m_game.board[row * N + column] = m_game.player;
            m_game.player = ((m_game.player - 1) ^ 1) + 1; // toggle between 1 and 2

            Game_over_condition();
        }
    }

    void Reset_game()
    {
        m_game.player = PLAYER_X;
        m_game.state = RUNNING_STATE;

        for (int i = 0; i < N * N; i++)
        {
            m_game.board[i] = EMPTY;
        }
    }

    void Click_on_cell(int row, int column)
    {
        switch (m_game.state)
        {
        case RUNNING_STATE:
            Player_turn(row, column);
            break;
        case X_WON_STATE:
        case O_WON_STATE:
        case TIE_STATE:
            m_game.state = DISPLAY_SCORE_STATE;
            break;
        default:
        {
            Reset_game();
        }
        }
    }
    void Render_X(int row, int column, const SDL_Color *color)
    {
        /*   __________
            |   ____   |
            |  |    |  |
            |  |____|  |
            |__________|
            |->| this is "half_box_side"
        */
        const float half_box_side = fmin(cell_width, cell_height) * 0.25;
        const float center_x = cell_width * 0.5 + column * cell_width;
        const float center_y = cell_height * 0.5 + row * cell_height;

        thickLineRGBA(GetRenderer(),
                      center_x - half_box_side,
                      center_y - half_box_side,
                      center_x + half_box_side,
                      center_y + half_box_side,
                      10,
                      color->r,
                      color->g,
                      color->b,
                      255);
        thickLineRGBA(GetRenderer(),
                      center_x + half_box_side,
                      center_y - half_box_side,
                      center_x - half_box_side,
                      center_y + half_box_side,
                      10,
                      color->r,
                      color->g,
                      color->b,
                      255);
    }

    void Render_O(int row, int column, const SDL_Color *color)
    {
        const float half_box_side = fmin(cell_width, cell_height) * 0.25;
        const float center_x = cell_width * 0.5 + column * cell_width;
        const float center_y = cell_height * 0.5 + row * cell_height;

        // (SDL_Surface *dst, Sint16 x, Sint16 y, Sint16 rad, Uint8 r, Uint8 g, Uint8 b, Uint8 a)
        filledCircleRGBA(GetRenderer(), center_x, center_y, half_box_side + 5, color->r, color->g, color->b, 255);
        filledCircleRGBA(GetRenderer(), center_x, center_y, half_box_side - 5, 0, 0, 0, 255);
    }

    void Render_grid(const SDL_Color *colour)
    {
        // SDL_SetRenderDrawColor(GetRenderer(), colour->r, colour->g, colour->b, 255);
        SetRenderDrawColour(*colour);

        for (int i = 1; i < N; ++i)
        {
            // start point (x1, y1) / end point (x2, y2)
            // int SDL_RenderDrawLine(SDL_Renderer * renderer, int x1, int y1, int x2, int y2);
            RenderDrawLine(i * cell_width, 0, i * cell_width, ScreenHeight());
            RenderDrawLine(0, i * cell_height, ScreenWidth(), i * cell_height);
        }
    }

    void Render_board(const SDL_Color *x_colour, const SDL_Color *o_colour)
    {
        /* board array is
            0 0 0
            0 0 0
            0 0 0
        */
        for (int i = 0; i < N; ++i)
        {
            for (int j = 0; j < N; ++j)
            {
                switch (m_game.board[i * N + j])
                {
                case PLAYER_X:
                    Render_X(i, j, x_colour);
                    break;

                case PLAYER_O:
                    Render_O(i, j, o_colour);
                    break;

                default:
                {
                }
                }
            }
        }
    }

    void Render_running_state()
    {
        Render_grid(&GRID_COLOUR);
        Render_board(&PLAYER_X_COLOUR, &PLAYER_O_COLOUR);
    }

    void Render_game_over_state(const SDL_Color *colour)
    {
        Render_grid(colour);
        Render_board(colour, colour);
    }

    void Render_score()
    {
        TTF_Font *font = TTF_OpenFont("..\\res\\fonts\\RobotoMono-Bold.ttf", 32);
        if (!font)
        {
            fprintf(stderr, "Error using TTF_OpenFont: %s\n", TTF_GetError());
            return;
        }

        char char_score[10];
        sprintf(char_score, "%d - %d", x_win_counter, o_win_counter);

        SDL_Surface *surface_score = TTF_RenderText_Blended(font, char_score, TEXT_COLOUR_WHITE);
        if (!surface_score)
        {
            fprintf(stderr, "Error using TTF_RenderText_Blended: %s\n", TTF_GetError());
            return;
        }
        SDL_Texture *texture_score = SDL_CreateTextureFromSurface(GetRenderer(), surface_score);

        // center text on screen
        const int x_pos = (SCREEN_WIDTH / 2) - (surface_score->w / 2);
        const int y_pos = (SCREEN_HEIGHT / 2) - (surface_score->h / 2);
        SDL_Rect score_rect_dest;             // create a rect
        score_rect_dest.x = x_pos;            // controls the rect's x coordinate
        score_rect_dest.y = y_pos;            // controls the rect's y coordinte
        score_rect_dest.w = surface_score->w; // controls the width of the rect
        score_rect_dest.h = surface_score->h; // controls the height of the rect

        SDL_RenderCopy(GetRenderer(), texture_score, NULL, &score_rect_dest);

        // Don't forget to free the surface and texture
        TTF_CloseFont(font);
        SDL_FreeSurface(surface_score);
        SDL_DestroyTexture(texture_score);
    }

    void Render_game()
    {
        switch (m_game.state)
        {
        case RUNNING_STATE:
            Render_running_state();
            break;
        case X_WON_STATE:
            Render_game_over_state(&PLAYER_X_COLOUR);
            break;
        case O_WON_STATE:
            Render_game_over_state(&PLAYER_O_COLOUR);
            break;
        case TIE_STATE:
            Render_game_over_state(&TIE_COLOUR);
            break;
        case DISPLAY_SCORE_STATE:
            Render_score();
            break;
        default:
            break;
        }
    }
};

int main(int argc, char *argv[])
{
    TicTacToe ttt;

    ttt.Init(SCREEN_WIDTH, SCREEN_HEIGHT);
    ttt.Start();

    return 1;
}