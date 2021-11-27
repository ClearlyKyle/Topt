#include <math.h>
#include <iostream>
#include <map>
// #include <stdint.h>
#include <string.h>
#include <chrono>
#include <SDL2/SDL.h>

/*
class EXAMPLE : public Topt::Rasterizer
{
private:

public:
    virtual bool OnUserCreate()
    {
        return true;
    }

    virtual bool OnUserUpdate(float fElapsedTime)
    {
        return true;
    }
};
*/

namespace Topt
{
    enum COLOUR
    {
        BLACK = 0,
        WHITE = 16777215,
        RED = 16711680,
        BLUE = 255,
        GREEN = 65280,
        YELLOW = 16776960
    }; /* COLOUR */

    /* Handle Keyboard Inputs */
    class Keyboard
    {
    private:
        Uint8 *m_Old_Keys;
        const Uint8 *m_New_Keys;
        int m_KeyLength;

    public:
        Keyboard()
        {
            m_New_Keys = SDL_GetKeyboardState(&m_KeyLength);
            m_Old_Keys = new Uint8[m_KeyLength];
            memcpy(m_Old_Keys, m_New_Keys, m_KeyLength);
        }
        void Update()
        {
            memcpy(m_Old_Keys, m_New_Keys, m_KeyLength);
        }
        bool Pressed(const SDL_Scancode code) const
        {
            return (m_Old_Keys[code] == 0 && m_New_Keys[code] != 0);
        }
        bool Triggered(const SDL_Scancode code) const
        {
            return (m_New_Keys[code] != 0);
        }
        bool Released(const SDL_Scancode code) const
        {
            return (m_Old_Keys[code] != 0 && m_New_Keys[code] == 0);
        }
    };

    class Mouse
    {
    private:
        Uint32 m_buttons;

    public:
        bool clicked = false;
        int m_x;
        int m_y;
        void Update()
        {
            // SDL_PumpEvents(); // make sure we have the latest mouse state.
            // m_buttons = SDL_GetMouseState(&m_x, &m_y);
        }
        int GetX() const
        {
            return m_x;
        }
        int GetY() const
        {
            return m_y;
        }
        /*  SDL_BUTTON_LEFT
            SDL_BUTTON_MIDDLE
            SDL_BUTTON_RIGHT
            SDL_BUTTON_X1
            SDL_BUTTON_X2
        */
        bool IsMouseButtonPressed(Uint32 button_mask)
        {
            return (m_buttons & button_mask) == 0 ? false : true;
        }
        bool SingleClick()
        {
            if (clicked)
            {
                clicked = false;
                return true;
            }
            return false;
        }
    };

    class Rasterizer
    {
    protected:
        uint32_t *m_FrameBuffer;

        unsigned int m_Width;
        unsigned int m_Height;
        unsigned int scale_x;
        unsigned int scale_y;

        Keyboard keyboard;
        Mouse mouse;

        SDL_Window *m_Window;
        SDL_Renderer *m_Renderer;
        SDL_Texture *m_Main_Texture;
        SDL_Event event;

        std::map<std::string, SDL_Texture *> m_Textures;

    public:
        unsigned int ScreenWidth() const { return m_Width; }
        unsigned int ScreenHeight() const { return m_Height; }
        SDL_Renderer *GetRenderer() { return m_Renderer; }

        void Init(unsigned int width, unsigned int height, const std::string &app_name = "Window", unsigned scale_x = 1, unsigned scale_y = 1)
        {
            m_Width = width;
            m_Height = height;

            SDL_Init(SDL_INIT_VIDEO);
            m_Window = SDL_CreateWindow("Window", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, width, height, SDL_WINDOW_SHOWN);
            m_Renderer = SDL_CreateRenderer(m_Window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
            m_Main_Texture = SDL_CreateTexture(m_Renderer, SDL_PIXELFORMAT_RGBA32, SDL_TEXTUREACCESS_STREAMING, m_Width / scale_x, m_Height / scale_x);
        }

        ~Rasterizer()
        {
            if (m_Textures.size() != 0)
            {
                for (auto t : m_Textures)
                {
                    SDL_DestroyTexture(t.second);
                }
            }

            SDL_DestroyTexture(m_Main_Texture);
            SDL_DestroyRenderer(m_Renderer);
            SDL_DestroyWindow(m_Window);
            SDL_Quit();
        }

        Uint32 GetPollEvent()
        {
            SDL_PollEvent(&event);
            return event.type;
        }

        void Start()
        {
            keyboard = Keyboard();

            OnUserCreate();

            auto tp1 = std::chrono::system_clock::now();
            auto tp2 = std::chrono::system_clock::now();

            bool g_Running = true;

            // loop until we're done running the program
            while (g_Running)
            {

                // Handle Timing
                tp2 = std::chrono::system_clock::now();
                std::chrono::duration<float> elapsedTime = tp2 - tp1;
                tp1 = tp2;
                float fElapsedTime = elapsedTime.count();

                while (SDL_PollEvent(&event))
                {
                    if (event.type == SDL_QUIT)
                    {
                        std::cout << "Quiting application\n";
                        g_Running = false; // set game state to done,(do what you want here)
                        break;
                    }
                    if (event.type == SDL_MOUSEBUTTONDOWN)
                    {

                        mouse.clicked = true;
                        mouse.m_y = event.button.y;
                        mouse.m_x = event.button.x;
                        break;
                    }
                    else if (event.type == SDL_MOUSEBUTTONUP)
                    {
                        mouse.clicked = false;
                        mouse.m_y = event.button.y;
                        mouse.m_x = event.button.x;
                        break;
                    }
                } // end of message processing

                void *pixels;
                int pitch;
                // lock a portion of the texture for write-only pixel access
                //      "&pixels" is a pointer to the locked pixels
                SDL_LockTexture(m_Main_Texture, NULL, &pixels, &pitch);

                m_FrameBuffer = (uint32_t *)pixels;
                memset(m_FrameBuffer, 0, sizeof(uint32_t) * (m_Height) * (m_Width));

                // Set blank draw colour and clear BEFORE!!! user updates
                SDL_SetRenderDrawColor(m_Renderer, 0, 0, 0, 255);
                SDL_RenderClear(m_Renderer);

                OnUserUpdate(fElapsedTime);
                keyboard.Update();
                // mouse.Update();

                // SDL_UnlockTexture(m_Main_Texture);

                // SDL_RenderCopy(m_Renderer, m_Main_Texture, NULL, NULL);
                SDL_RenderPresent(m_Renderer);
            }
        }

        void SetRenderDrawColour(SDL_Color colour, int alpha = 255)
        {
            SDL_SetRenderDrawColor(m_Renderer, colour.r, colour.g, colour.b, 255);
        }
        void RenderDrawLine(int x1, int y1, int x2, int y2)
        {
            SDL_RenderDrawLine(m_Renderer, x1, y1, x2, y2);
        }

        void SetPixel(unsigned int x, unsigned int y, const Topt::COLOUR &colour)
        {
            if (x >= m_Width || y >= m_Height)
                return;

            m_FrameBuffer[y * m_Width + x] = colour;
        }

        void SetPixel(unsigned int x, unsigned int y, Uint32 colour)
        {
            if (x >= m_Width || y >= m_Height)
                return;

            m_FrameBuffer[y * m_Width + x] = colour;
        }

        void DrawLine(float x1, float y1, float x2, float y2, Topt::COLOUR colour = Topt::COLOUR::WHITE)
        {
            float xdiff = (x2 - x1);
            float ydiff = (y2 - y1);

            if (xdiff == 0.0f && ydiff == 0.0f)
            {
                SetPixel(x1, y1, colour);
                return;
            }

            if (fabs(xdiff) > fabs(ydiff))
            {
                float xmin, xmax;

                // set xmin to the lower x value given
                // and xmax to the higher value
                if (x1 < x2)
                {
                    xmin = x1;
                    xmax = x2;
                }
                else
                {
                    xmin = x2;
                    xmax = x1;
                }

                // draw line in terms of y slope
                float slope = ydiff / xdiff;
                for (float x = xmin; x <= xmax; x += 1.0f)
                {
                    float y = y1 + ((x - x1) * slope);
                    SetPixel(x, y, colour);
                }
            }
            else
            {
                float ymin, ymax;

                // set ymin to the lower y value given
                // and ymax to the higher value
                if (y1 < y2)
                {
                    ymin = y1;
                    ymax = y2;
                }
                else
                {
                    ymin = y2;
                    ymax = y1;
                }

                // draw line in terms of x slope
                float slope = xdiff / ydiff;
                for (float y = ymin; y <= ymax; y += 1.0f)
                {
                    float x = x1 + ((y - y1) * slope);
                    SetPixel(x, y, colour);
                }
            }
        }

    public:
        // User MUST OVERRIDE THESE!!
        virtual bool OnUserCreate() = 0;
        virtual bool OnUserUpdate(float fElapsedTime) = 0;
    };

    class Texture
    {
    private:
        SDL_Surface *m_Surface;
        SDL_Texture *m_Texture;

    public:
        void LoadTexture(const std::string &path)
        {
            m_Surface = SDL_LoadBMP(path.c_str()); // path relative to .exe file
            if (m_Surface == nullptr)
            {
                std::cout << "Failed to load surface " << path << " error : " << SDL_GetError() << std::endl;
            }
            // m_Texture = SDL_CreateTextureFromSurface(m_Renderer, m_Surface);
        }

        // pos - {x, y, w, h}
        void Render(SDL_Rect pos)
        {
            /* This is moved to Rasterizer Class due to needing Renderer */
            // SDL_RenderCopy(m_Renderer, m_Texture, NULL, &pos);
        }

        Uint32 GetPixel(int x, int y)
        {
            int bpp = m_Surface->format->BytesPerPixel;
            Uint8 *p = (Uint8 *)m_Surface->pixels + y * m_Surface->pitch + x * bpp;
            /*
            1   Uint8
            2   Uint16
            3   tuple of Uint8 RGB values
            4   Uint32
            */
            // bpp = 4;
            switch (bpp)
            {
            case 1:
                return *p;
                break;
            case 2:
                return *(Uint16 *)p;
                break;
            case 3:
                if (SDL_BYTEORDER == SDL_BIG_ENDIAN)
                    return p[0] << 16 | p[1] << 8 | p[2];
                else
                    return p[0] | p[1] << 8 | p[2] << 16;
                break;
            case 4:
                return *(Uint32 *)p;
                break;
            default:
                return 0; /* shouldn't happen, but avoids warnings */
            }
        }
    };
} // namespace Topt