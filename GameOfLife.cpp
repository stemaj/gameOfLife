#define OLC_PGE_APPLICATION
#include "olcPixelGameEngine.h"

#include <random>
int wuerfel(int von, int bis)
{
  std::random_device rd;  //Will be used to obtain a seed for the random number engine
  std::mt19937 gen(rd()); //Standard mersenne_twister_engine seeded with rd()
  std::uniform_int_distribution<> wuerfeln(von, bis);
  return wuerfeln(gen);
}

// Override base class with your custom functionality
class GameOfLife : public olc::PixelGameEngine
{
public:
	GameOfLife()
	{
		// Name your application
		sAppName = "GameOfLife";
	}

  std::map<olc::vi2d, bool> grid;

  olc::Sprite *spriteLive;
  olc::Sprite *spriteDead;
  olc::Decal* decalLive;
  olc::Decal* decalDead;

public:
	bool OnUserCreate() override
	{
    for (int x = 0; x < ScreenWidth(); x++)
    {
      for (int y = 0; y < ScreenHeight(); y++)
      {
        grid[olc::vi2d(x,y)] = false;
      }
    }

    for (int x = ScreenWidth() / 4; x < 3 * ScreenWidth() / 4; x++)
    {
      for (int y = ScreenHeight() / 4; y < 3 * ScreenHeight() / 4; y++)
      {
        grid[olc::vi2d(x,y)] = wuerfel(1,3) == 1 || wuerfel(1,3) == 2;
      }
    }

    spriteLive = new olc::Sprite(1,1);
    spriteLive->SetPixel(0,0,olc::CYAN);
    decalLive = new olc::Decal(spriteLive);

    spriteDead = new olc::Sprite(1,1);
    spriteDead->SetPixel(0,0,olc::DARK_BLUE);
    decalDead = new olc::Decal(spriteDead);

    for (const auto& g : grid)
    {
      //DrawDecal(g.first, g.second ? decalLive : decalDead);
      Draw(g.first.x, g.first.y, g.second ? olc::CYAN : olc::DARK_BLUE);
    }

		return true;
	}

  int getPrevX(const int x)
  {
    if (x == 0) return ScreenWidth();
    else return (x-1);
  }

  int getPrevY(const int y)
  {
    if (y == 0) return ScreenHeight();
    else return (y-1);
  }

  int getNextX(const int x)
  {
    if (x == ScreenWidth()) return 0;
    else return (x+1);
  }

  int getNextY(const int y)
  {
    if (y == ScreenHeight()) return 0;
    else return (y+1);
  }

  int livingNeighbors(olc::vi2d cell)
  {
    int sum = 0;
    sum += grid[olc::vi2d(getPrevX(cell.x),getPrevY(cell.y))];
    sum += grid[olc::vi2d(getPrevX(cell.x),         cell.y )];
    sum += grid[olc::vi2d(getPrevX(cell.x),getNextY(cell.y))];
    sum += grid[olc::vi2d(         cell.x ,getPrevY(cell.y))];
    sum += grid[olc::vi2d(         cell.x ,getNextY(cell.y))];
    sum += grid[olc::vi2d(getNextX(cell.x),getPrevY(cell.y))];
    sum += grid[olc::vi2d(getNextX(cell.x),         cell.y )];
    sum += grid[olc::vi2d(getNextX(cell.x),getNextY(cell.y))];
    return sum;
  }

	bool OnUserUpdate(float fElapsedTime) override
	{
    if (GetKey(olc::ESCAPE).bPressed) exit (0);

    std::map<olc::vi2d, bool> updatedGrid;

    for (const auto& g : grid)
    {
      int neig = livingNeighbors(g.first);
      bool WillBeLiving;
      if (g.second) // living
      {
        if (neig == 2 || neig == 3)
        {
          // survival
          WillBeLiving = true;
        }
        else
        {
          // under-, overpopulation
          WillBeLiving = false;
        }
      }
      else // dead
      {
        if (neig == 3)
        {
          // reproduction
          WillBeLiving = true;
        }
        else
        {
          // stay dead
          WillBeLiving = false;
        }
      }
      updatedGrid[g.first] = WillBeLiving;
    }

    grid = updatedGrid;
    for (const auto& g : grid)
    {
      //DrawDecal(g.first, g.second ? decalLive : decalDead);
      Draw(g.first.x, g.first.y, g.second ? olc::CYAN : olc::DARK_BLUE);
    }
		return true;
	}
};

int main()
{
	GameOfLife demo;
	if (demo.Construct(120, 75, 8, 8))
		demo.Start();
	return 0;
}
