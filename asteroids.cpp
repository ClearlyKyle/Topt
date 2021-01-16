#include <vector>
#include <algorithm>

#include "topt.h"

class Asteriods : public Topt::Rasterizer
{
    private:
        struct sSpaceObject 
        {
            float x;
            float y;
            float dy;
            float dx;
            int nSize;
            float angle;
        };

        std::vector<sSpaceObject> vecAsteroids;
        std::vector<sSpaceObject> vecBullets;
        sSpaceObject player;

        std::vector<std::pair<float, float>> vecModelShip;
        std::vector<std::pair<float, float>> vecModelAsteroid;

    public:
        virtual bool OnUserCreate()
        {
            // vecAsteroids.push_back({20.0f, 20.0f, 8.0f, 6.0f, (int)16, 0.0f});

            player.x = ScreenWidth() / 2,
            player.y = ScreenHeight() / 2,
            player.dx = 0.0f;
            player.dy = 0.0f;
            player.angle = 0.0f;

            vecModelShip =
            {
                { 0.0f, -5.0f},
                {-2.5f, +2.5f},
                {+2.5f, +2.5f}
            };

            int verts = 20;
            for (int i = 0; i < verts; i++)
            {
                float noise = (float)rand() / (float)RAND_MAX * 0.4f + 0.8f;
                vecModelAsteroid.push_back(std::make_pair(  noise * sinf(((float)i / (float)verts) * 6.28318f), 
                                                            noise * cosf(((float)i / (float)verts) * 6.28318f)));
            }

            // Put in two asteroids
            vecAsteroids.push_back({  20.0f, 20.0f, 8.0f, -6.0f, (int)16, 0.0f });
            vecAsteroids.push_back({ 100.0f, 20.0f, -5.0f, 3.0f, (int)16, 0.0f });

            return true;
        }

        virtual bool OnUserUpdate(float fElapsedTime)
        {
            // Player control
            if(keyboard.Triggered(SDL_SCANCODE_LEFT))
                player.angle -= 5.0f * fElapsedTime;
            if(keyboard.Triggered(SDL_SCANCODE_RIGHT))
                player.angle += 5.0f * fElapsedTime;
                
            if(keyboard.Triggered(SDL_SCANCODE_UP))
            {
                player.dx += sin(player.angle) * 20.0f * fElapsedTime;
                player.dy += -cos(player.angle) * 20.0f * fElapsedTime;
            }

            player.x += player.dx * fElapsedTime;
            player.y += player.dy * fElapsedTime;

            WrapCoordinates(player.x, player.y, player.x, player.y);
            
            // Fire bullets
            if(keyboard.Released(SDL_SCANCODE_SPACE))
            {
                std::cout << "BULLET\n";
                vecBullets.push_back({player.x, player.y - 5.0f, -50.0f * cosf(player.angle),  50.0f * sinf(player.angle), 0, 100.0f});
            }

            // Drawing Asteroids
            for (auto &a : vecAsteroids)
            {
                a.x += a.dx * fElapsedTime;
                a.y += a.dy * fElapsedTime;
                WrapCoordinates(a.x, a.y, a.x, a.y);

                DrawWireFrame(vecModelAsteroid, a.x, a.y, a.angle, a.nSize);
            }
            
            std::vector<sSpaceObject> newAsteroids;

            // Drawing Bullets
            for (auto &b : vecBullets)
            {
                b.x += b.dx * fElapsedTime;
                b.y += b.dy * fElapsedTime;
                WrapCoordinates(b.x, b.y, b.x, b.y);

                for (auto &a : vecAsteroids)
                {
                    if(IsPointInsideCircle(a.x, a.y, a.nSize, b.x, b.y))
                    {
                        // Asteroid was hit by a bullet
                        b.x = -100;
                        // Create child asteroids
                        if (a.nSize > 4)
                        {
                            float angle1 = ((float)rand() / (float)RAND_MAX) * 6.283185f;
                            float angle2 = ((float)rand() / (float)RAND_MAX) * 6.283185f;
                            newAsteroids.push_back({ a.x, a.y, 10.0f * sinf(angle1), 10.0f * cosf(angle1), (int)a.nSize >> 1, 0.0f });
                            newAsteroids.push_back({ a.x, a.y, 10.0f * sinf(angle2), 10.0f * cosf(angle2), (int)a.nSize >> 1, 0.0f });
                        }
                        a.x = -100;
                    }
                    // Remove asteroid - Same approach as bullets
					
                }

                SetPixel(b.x, b.y, Topt::COLOUR::WHITE);
            }

            // Append new asteroids to existing vector
		    for(auto a:newAsteroids)
			    vecAsteroids.push_back(a);

            // Remove off screen bullets
            if (vecBullets.size() != 0)
            {
                auto i = std::remove_if(vecBullets.begin(),
                                        vecBullets.end(),
                                        [&](sSpaceObject o){return o.x < 1 || o.y < 1 || o.x >= ScreenWidth() || o.y >= ScreenHeight(); });
            }

            // Remove asteroids that have been blown up
            if (vecAsteroids.size() != 0)
            {
                auto i = remove_if( vecAsteroids.begin(), 
                                    vecAsteroids.end(), 
                                    [&](sSpaceObject o) { return (o.x < 0); });

                if (i != vecAsteroids.end())
                    vecAsteroids.erase(i);
            }

            if (vecAsteroids.empty()) // If no asteroids, level complete! :) - you win MORE asteroids!
            {
                // Level Clear
                vecAsteroids.clear();
                vecBullets.clear();

                vecAsteroids.push_back({    30.0f * sinf(player.angle - 3.14159f/2.0f) + player.x,
										    30.0f * cosf(player.angle - 3.14159f/2.0f) + player.y,
											10.0f * sinf(player.angle), 
                                            10.0f * cosf(player.angle), 
                                            (int)16,
                                            0.0f
                                            });

                vecAsteroids.push_back({    30.0f * sinf(player.angle + 3.14159f/2.0f) + player.x,
                                            30.0f * cosf(player.angle + 3.14159f/2.0f) + player.y,
                                            10.0f * sinf(-player.angle), 
                                            10.0f * cosf(-player.angle),
                                            (int)16, 
                                            0.0f
                                            });
            }

            // Drawing Player
            DrawWireFrame(vecModelShip, player.x, player.y, player.angle, 2.0f);

            return true;
        }
        bool IsPointInsideCircle(float cx, float cy, float radius, float x, float y)
        {
            return sqrt((x-cx)*(x-cx) + (y-cy)*(y-cy)) < radius;
        }

        // TODO : Update this function 
        void DrawWireFrame(const std::vector<std::pair<float, float>> &vecModelCoordinates, float x, float y, float r = 0.0f, float s = 1.0f, Topt::COLOUR col = Topt::COLOUR::WHITE)
        {
            // pair.first = x coordinate
            // pair.second = y coordinate

            // Create translated model vector of coordinate pairs
            std::vector<std::pair<float, float>> vecTransformedCoordinates;
            int verts = vecModelCoordinates.size();
            vecTransformedCoordinates.resize(verts);

            // Rotate
            for (int i = 0; i < verts; i++)
            {
                vecTransformedCoordinates[i].first = vecModelCoordinates[i].first * cosf(r) - vecModelCoordinates[i].second * sinf(r);
                vecTransformedCoordinates[i].second = vecModelCoordinates[i].first * sinf(r) + vecModelCoordinates[i].second * cosf(r);
            }

            // Scale
            for (int i = 0; i < verts; i++)
            {
                vecTransformedCoordinates[i].first = vecTransformedCoordinates[i].first * s;
                vecTransformedCoordinates[i].second = vecTransformedCoordinates[i].second * s;
            }

            // Translate
            for (int i = 0; i < verts; i++)
            {
                vecTransformedCoordinates[i].first = vecTransformedCoordinates[i].first + x;
                vecTransformedCoordinates[i].second = vecTransformedCoordinates[i].second + y;
            }

            // Draw Closed Polygon
            for (int i = 0; i < verts + 1; i++)
            {
                int j = (i + 1);
                DrawLine((int)vecTransformedCoordinates[i % verts].first, (int)vecTransformedCoordinates[i % verts].second,
                    (int)vecTransformedCoordinates[j % verts].first, (int)vecTransformedCoordinates[j % verts].second);
            }
        }

        void WrapCoordinates(float ix, float iy, float &ox, float &oy)
        {
            ox = ix;
            oy = iy;

            if(ix < 0.01f) ox = ix + (float)m_Width;
            if(ix > (float)m_Width) ox = ix - (float)m_Width;

            if(iy < 0.01f) oy = iy + (float)m_Height;
            if(iy > (float)m_Height) oy = iy - (float)m_Height;
        }

        void SetPixel(unsigned int x, unsigned int y, const Topt::COLOUR &colour)
        {
            float fx, fy;
            WrapCoordinates(x, y, fx, fy);
            Topt::Rasterizer::SetPixel(fx, fy, colour);
        }
};

int main(int argc, char *argv[])
{
    Asteriods asteriod;

    asteriod.Init(800, 800);
    asteriod.Start();

    return 1;
}