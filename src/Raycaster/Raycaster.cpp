#include <iostream>
#include <vector>
#include "TOPT.h"

int screenWidth = 640;
int screenHeight = 480;

class RayCaster : public Topt::Rasterizer
{
private:
    int mapWidth = 24;
    int mapHeight = 24;

    int worldMap[24][24] =
        {
            {4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 7, 7, 7, 7, 7, 7, 7, 7},
            {4, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 7, 0, 0, 0, 0, 0, 0, 7},
            {4, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 7},
            {4, 0, 2, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 7},
            {4, 0, 3, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 7, 0, 0, 0, 0, 0, 0, 7},
            {4, 0, 4, 0, 0, 0, 0, 5, 5, 5, 5, 5, 5, 5, 5, 5, 7, 7, 0, 7, 7, 7, 7, 7},
            {4, 0, 5, 0, 0, 0, 0, 5, 0, 5, 0, 5, 0, 5, 0, 5, 7, 0, 0, 0, 7, 7, 7, 1},
            {4, 0, 6, 0, 0, 0, 0, 5, 0, 0, 0, 0, 0, 0, 0, 5, 7, 0, 0, 0, 0, 0, 0, 8},
            {4, 0, 7, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 7, 7, 7, 1},
            {4, 0, 8, 0, 0, 0, 0, 5, 0, 0, 0, 0, 0, 0, 0, 5, 7, 0, 0, 0, 0, 0, 0, 8},
            {4, 0, 0, 0, 0, 0, 0, 5, 0, 0, 0, 0, 0, 0, 0, 5, 7, 0, 0, 0, 7, 7, 7, 1},
            {4, 0, 0, 0, 0, 0, 0, 5, 5, 5, 5, 0, 5, 5, 5, 5, 7, 7, 7, 7, 7, 7, 7, 1},
            {6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 0, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6},
            {8, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 4},
            {6, 6, 6, 6, 6, 6, 0, 6, 6, 6, 6, 0, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6},
            {4, 4, 4, 4, 4, 4, 0, 4, 4, 4, 6, 0, 6, 2, 2, 2, 2, 2, 2, 2, 3, 3, 3, 3},
            {4, 0, 0, 0, 0, 0, 0, 0, 0, 4, 6, 0, 6, 2, 0, 0, 0, 0, 0, 2, 0, 0, 0, 2},
            {4, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 6, 2, 0, 0, 5, 0, 0, 2, 0, 0, 0, 2},
            {4, 0, 0, 0, 0, 0, 0, 0, 0, 4, 6, 0, 6, 2, 0, 0, 0, 0, 0, 2, 2, 0, 2, 2},
            {4, 0, 6, 0, 6, 0, 0, 0, 0, 4, 6, 0, 0, 0, 0, 0, 5, 0, 0, 0, 0, 0, 0, 2},
            {4, 0, 0, 5, 0, 0, 0, 0, 0, 4, 6, 0, 6, 2, 0, 0, 0, 0, 0, 2, 2, 0, 2, 2},
            {4, 0, 6, 0, 6, 0, 0, 0, 0, 4, 6, 0, 6, 2, 0, 0, 5, 0, 0, 2, 0, 0, 0, 2},
            {4, 0, 0, 0, 0, 0, 0, 0, 0, 4, 6, 0, 6, 2, 0, 0, 0, 0, 0, 2, 0, 0, 0, 2},
            {4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 1, 1, 1, 2, 2, 2, 2, 2, 2, 3, 3, 3, 3, 3}};

    // The ratio between the length of the direction and the camera plane determinates the FOV
    //  (more precisely, the FOV is 2 * atan(planeY/1.0)=66°
    double posX = 22.0, posY = 11.5;    // x and y start position
    double dirX = -1.0, dirY = 0.0;     // initial direction vector
    double planeX = 0.0, planeY = 0.66; // the 2d raycaster version of camera plane

    double time = 0;    // time of current frame
    double oldTime = 0; // time of previous frame

    Topt::Texture texs[8];

public:
    RayCaster()
    {
        texs[0].LoadTexture("../res/tex1.bmp");
        texs[1].LoadTexture("../res/tex2.bmp");
        texs[4].LoadTexture("../res/tex3.bmp");
        texs[3].LoadTexture("../res/tex4.bmp");
        texs[2].LoadTexture("../res/tex5.bmp");
        texs[5].LoadTexture("../res/tex6.bmp");
        texs[6].LoadTexture("../res/tex7.bmp");
        texs[7].LoadTexture("../res/tex8.bmp");
    }
    virtual bool OnUserCreate()
    {
        return true;
    }

    virtual bool OnUserUpdate(float fElapsedTime)
    {
        Uint32 buffer[screenHeight][screenWidth]; // y-coordinate first because it works per scanline

        const int texWidth = 64;
        const int texHeight = 64;
        std::cout << "planeX = " << planeX << " planeY = " << planeY << std::endl;

        for (int x = 0; x < screenWidth; x++)
        {
            // calculate ray position and direction
            const double cameraX = 2 * x / double(screenWidth) - 1; // x-coordinate in camera space
            const double rayDirX = dirX + planeX * cameraX;
            const double rayDirY = dirY + planeY * cameraX;

            // which box of the map we're in
            int mapX = int(posX);
            int mapY = int(posY);

            // length of ray from current position to next x or y-side
            double sideDistX;
            double sideDistY;

            // length of ray from one x or y-side to next x or y-side
            const double deltaDistX = std::abs(1 / rayDirX);
            const double deltaDistY = std::abs(1 / rayDirY);
            double perpWallDist;

            // what direction to step in x or y-direction (either +1 or -1)
            int stepX;
            int stepY;

            int hit = 0; // was there a wall hit?
            int side;    // was a NS or a EW wall hit?

            // calculate step and initial sideDist
            if (rayDirX < 0)
            {
                stepX = -1;
                sideDistX = (posX - mapX) * deltaDistX;
            }
            else
            {
                stepX = 1;
                sideDistX = (mapX + 1.0 - posX) * deltaDistX;
            }
            if (rayDirY < 0)
            {
                stepY = -1;
                sideDistY = (posY - mapY) * deltaDistY;
            }
            else
            {
                stepY = 1;
                sideDistY = (mapY + 1.0 - posY) * deltaDistY;
            }
            // perform DDA
            while (hit == 0)
            {
                // jump to next map square, OR in x-direction, OR in y-direction
                if (sideDistX < sideDistY)
                {
                    sideDistX += deltaDistX;
                    mapX += stepX;
                    side = 0;
                }
                else
                {
                    sideDistY += deltaDistY;
                    mapY += stepY;
                    side = 1;
                }
                // Check if ray has hit a wall
                if (worldMap[mapX][mapY] > 0)
                    hit = 1;
            } /* end of DDA */

            // Calculate distance projected on camera direction (Euclidean distance will give fisheye effect!)
            if (side == 0)
                perpWallDist = (mapX - posX + (1 - stepX) / 2) / rayDirX;
            else
                perpWallDist = (mapY - posY + (1 - stepY) / 2) / rayDirY;

            // Calculate height of line to draw on screen
            int h = screenHeight;
            int lineHeight = (int)(h / perpWallDist);

            // calculate lowest and highest pixel to fill in current stripe
            int drawStart = -lineHeight / 2 + h / 2;
            if (drawStart < 0)
                drawStart = 0;
            int drawEnd = lineHeight / 2 + h / 2;
            if (drawEnd >= h)
                drawEnd = h - 1;

            // texturing calculations
            int texNum = worldMap[mapX][mapY] - 1; // 1 subtracted from it so that texture 0 can be used!

            // calculate value of wallX
            double wallX; // where exactly the wall was hit
            if (side == 0)
                wallX = posY + perpWallDist * rayDirY;
            else
                wallX = posX + perpWallDist * rayDirX;
            wallX -= floor((wallX));

            // x coordinate on the texture
            int texX = int(wallX * double(texWidth));
            if (side == 0 && rayDirX > 0)
                texX = texWidth - texX - 1;
            if (side == 1 && rayDirY < 0)
                texX = texWidth - texX - 1;

            // TODO: an integer-only bresenham or DDA like algorithm could make the texture coordinate stepping faster
            // How much to increase the texture coordinate per screen pixel
            double step = 1.0 * texHeight / lineHeight;
            // Starting texture coordinate
            double texPos = (drawStart - h / 2 + lineHeight / 2) * step;
            for (int y = drawStart; y < drawEnd; y++)
            {
                // Cast the texture coordinate to integer, and mask with (texHeight - 1) in case of overflow
                const int texY = (int)texPos & (texHeight - 1);
                texPos += step;
                Uint32 color = texs[texNum].GetPixel(texX, texY);
                // make color darker for y-sides: R, G and B byte each divided through two with a "shift" and an "and"
                if (side == 1)
                    color = (color >> 1) & 8355711;
                buffer[y][x] = color;
                SetPixel(x, y, buffer[y][x]);
            }
        }

        // speed modifiers
        const double moveSpeed = fElapsedTime * 5.0; // the constant value is in squares/second
        const double rotSpeed = fElapsedTime * 3.0;  // the constant value is in radians/second

        // move forward if no wall in front of you
        if (keyboard.Triggered(SDL_SCANCODE_UP))
        {
            if (worldMap[int(posX + dirX * moveSpeed)][int(posY)] == false)
                posX += dirX * moveSpeed;
            if (worldMap[int(posX)][int(posY + dirY * moveSpeed)] == false)
                posY += dirY * moveSpeed;
        }
        // move backwards if no wall behind you
        if (keyboard.Triggered(SDL_SCANCODE_DOWN))
        {
            if (worldMap[int(posX - dirX * moveSpeed)][int(posY)] == false)
                posX -= dirX * moveSpeed;
            if (worldMap[int(posX)][int(posY - dirY * moveSpeed)] == false)
                posY -= dirY * moveSpeed;
        }
        // rotate to the right
        if (keyboard.Triggered(SDL_SCANCODE_RIGHT))
        {
            // both camera direction and camera plane must be rotated
            const double oldDirX = dirX;
            dirX = dirX * cos(-rotSpeed) - dirY * sin(-rotSpeed);
            dirY = oldDirX * sin(-rotSpeed) + dirY * cos(-rotSpeed);
            const double oldPlaneX = planeX;
            planeX = planeX * cos(-rotSpeed) - planeY * sin(-rotSpeed);
            planeY = oldPlaneX * sin(-rotSpeed) + planeY * cos(-rotSpeed);
        }
        // rotate to the left
        if (keyboard.Triggered(SDL_SCANCODE_LEFT))
        {
            // both camera direction and camera plane must be rotated
            const double oldDirX = dirX;
            dirX = dirX * cos(rotSpeed) - dirY * sin(rotSpeed);
            dirY = oldDirX * sin(rotSpeed) + dirY * cos(rotSpeed);
            const double oldPlaneX = planeX;
            planeX = planeX * cos(rotSpeed) - planeY * sin(rotSpeed);
            planeY = oldPlaneX * sin(rotSpeed) + planeY * cos(rotSpeed);
        }

        return true; /* Rreturn from OnUserUpdate */
    }
};

int main(int argc, char *argv[])
{
    RayCaster rc;

    rc.Init(640, 480);
    rc.Start();

    return 1;
}