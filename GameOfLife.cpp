//#define RUN_TESTS
#ifdef RUN_TESTS
  #define DOCTEST_CONFIG_IMPLEMENT
  #include "doctest.h"
#endif

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

  std::vector<char> grid;
  int w = 0;
  int h = 0;

  std::vector<char> updatedGrid;

public:
	bool OnUserCreate() override
	{
    w = ScreenWidth();
    h = ScreenHeight();
    grid.resize(w*h);

    // init some random
    for (int x = w / 4; x < 3 * w / 4; x++)
    {
      for (int y = h / 4; y < 3 * h / 4; y++)
      {
        grid[y*w+x] = (wuerfel(1,3) == 1 || wuerfel(1,3) == 2) ? 1 : 0;
      }
    }

    DrawGrid();
		return true;
	}

  char onUpperEdge(int val)
  {
    return (val < w) ? 1 : 0;
  }
  char onLeftEdge(int val)
  {
    return (val%w == 0) ? 1 : 0;
  }
  char onRightEdge(int val)
  {
    return ((val+1)%w == 0) ? 1 : 0;
  }
  char onLowerEdge(int val)
  {
    return ( val >= (h-1)*w ) ? 1 : 0;
  }

  char valueUpperLeft(int pos)
  {
    if (onUpperEdge(pos) && onLeftEdge(pos)) return grid[(w*h)-1];
    else if (onUpperEdge(pos)) return grid[w*(h-1)+pos-1];
    else if (onLeftEdge(pos)) return grid[pos-1];
    return grid[pos-w-1];
  }

  char valueLeft(int pos)
  {
    if (onLeftEdge(pos)) return grid[pos+w-1];
    return grid[pos-1];
  }

  char valueLowerLeft(int pos)
  {
    if (onLowerEdge(pos) && onLeftEdge(pos)) return grid[w-1];
    else if (onLeftEdge(pos)) return grid[pos+2*w-1];
    else if (onLowerEdge(pos)) return grid[pos-(h-1)*w-1];  
    return grid[pos+w-1];
  }

  char valueLow(int pos)
  {
    if (onLowerEdge(pos)) return grid[pos-w*(h-1)];
    return grid[pos+w];
  }

  char valueLowerRight(int pos)
  {
    if (onLowerEdge(pos) && onRightEdge(pos)) return grid[0];
    else if (onRightEdge(pos)) return grid[pos+1];
    else if (onLowerEdge(pos)) return grid[pos-(h-1)*w+1];
    return grid[pos+w+1];
  }

  char valueRight(int pos)
  {
    if (onRightEdge(pos)) return grid[pos-w+1];
    return grid[pos+1];
  }

  char valueUpperRight(int pos)
  {
    if (onUpperEdge(pos) && onRightEdge(pos)) return grid[w*(h-1)];
    else if (onRightEdge(pos)) return grid[pos-2*w+1];
    else if (onUpperEdge(pos)) return grid[pos+w*(h-1)+1];
    return grid[pos-w+1];
  }
  
  char valueUp(int pos)
  {
    if (onUpperEdge(pos)) return grid[pos+w*(h-1)];
    return grid[pos-w];
  }

  int livingNeighbors(int val)
  {
    int sum = 0;
    sum += (valueUpperLeft( val)) ? 1 : 0;
    sum += (valueLeft(      val)) ? 1 : 0;
    sum += (valueLowerLeft( val)) ? 1 : 0;
    sum += (valueLow(       val)) ? 1 : 0;
    sum += (valueLowerRight(val)) ? 1 : 0;
    sum += (valueRight(     val)) ? 1 : 0;
    sum += (valueUpperRight(val)) ? 1 : 0;
    sum += (valueUp(        val)) ? 1 : 0;
    return sum;
  }

  void DoIteration(std::vector<char>& updatedGrid)
  {
    auto size = grid.size();
    for (int i = 0; i < size; i++)
    {
      int neig = livingNeighbors(i);
      char WillBeLiving;
      if (grid[i] == char(1)) // living
      {
        if (neig == 2 || neig == 3)
        {
          // survival
          WillBeLiving = char(1);
        }
        else
        {
          // under-, overpopulation
          WillBeLiving = char(0);
        }
      }
      else // dead
      {
        if (neig == 3)
        {
          // reproduction
          WillBeLiving = char(1);
        }
        else
        {
          // stay dead
          WillBeLiving = char(0);
        }
      }
      updatedGrid.emplace_back(WillBeLiving);
    }
  }

	bool OnUserUpdate(float fElapsedTime) override
	{
    if (GetKey(olc::ESCAPE).bPressed) return false;

    std::vector<char> updatedGrid;
    updatedGrid.reserve(w*h);
    DoIteration(updatedGrid);
    
    grid.assign(updatedGrid.begin(), updatedGrid.end()); 
    DrawGrid();
    
		return true;
	}
private:
  void DrawGrid()
  {
    for (int y = 0; y < h; y++)
    {
      for (int x = 0; x < w; x++)
      {
        Draw(x,y, grid[y*w+x] == 1 ? olc::CYAN : olc::DARK_BLUE);
      }
    }
  }
};

int main()
{
#ifdef RUN_TESTS
  doctest::Context context;
  context.run(); 
#else
	GameOfLife demo;
	if (demo.Construct(120*4, 75*4, 8/4, 8/4))
		demo.Start();
	return 0;
#endif
}

#ifdef RUN_TESTS

TEST_CASE("living neighbors") {
  GameOfLife test;
  test.w = 4;
  test.h = 3;
  test.grid = { 0, 0, 1, 0, 1, 0, 0, 1, 0, 1, 1, 0 };

  /*
  .    .  o  o  .    .

  .    .  .  o  .    .
  o    o  .  .  o    o 
  .    .  o  o  .    .

  .    .  .  o  .    .
  */

  CHECK(test.valueUpperRight(3) == 0);
  CHECK(test.valueRight(3) == 0);
  CHECK(test.valueLowerRight(3) == 1);
  CHECK(test.valueUpperLeft(3) == 1);
  CHECK(test.valueLeft(3) == 1);
  CHECK(test.valueLowerLeft(3) == 0);
  CHECK(test.valueUp(3) == 0);
  CHECK(test.valueLow(3) == 1);

  CHECK(test.valueUpperRight(8) == 0);
  CHECK(test.valueRight(8) == 1);
  CHECK(test.valueLowerRight(8) == 0);
  CHECK(test.valueUpperLeft(8) == 1);
  CHECK(test.valueLeft(8) == 0);
  CHECK(test.valueLowerLeft(8) == 0);
  CHECK(test.valueUp(8) == 1);
  CHECK(test.valueLow(8) == 0);
  
  CHECK(test.livingNeighbors(0) == 3);// t  1
  CHECK(test.livingNeighbors(1) == 4);// t
  CHECK(test.livingNeighbors(2) == 3);//    1
  CHECK(test.livingNeighbors(3) == 4);// t
  CHECK(test.livingNeighbors(4) == 2);//    1
  CHECK(test.livingNeighbors(5) == 4);// t
  CHECK(test.livingNeighbors(6) == 4);// t
  CHECK(test.livingNeighbors(7) == 3);//    1
  CHECK(test.livingNeighbors(8) == 3);// t  1
  CHECK(test.livingNeighbors(9) == 3);//    1
  CHECK(test.livingNeighbors(10) == 3);//   1
  CHECK(test.livingNeighbors(11) == 4);// t 


  std::vector<char> updatedGrid;
  test.DoIteration(updatedGrid);
  CHECK(updatedGrid.size() == test.w*test.h);

  CHECK(updatedGrid == std::vector<char> { 1, 0, 1, 0, 1, 0, 0, 1, 1, 1, 1, 0 } );
  
  CHECK(test.grid == std::vector<char>   { 0, 0, 1, 0, 1, 0, 0, 1, 0, 1, 1, 0 } );
  test.grid.assign(updatedGrid.begin(), updatedGrid.end()); 
  CHECK(test.grid == std::vector<char>   { 1, 0, 1, 0, 1, 0, 0, 1, 1, 1, 1, 0 } );
}

TEST_CASE("test 3x2 grid positions") {
  GameOfLife test;
  test.w = 3;
  test.h = 2;
  test.grid = { 0, 1, 2, 3, 4, 5 };
  CHECK(test.grid.size() == test.w*test.h);
  CHECK(test.grid[test.grid.size()-1] == test.grid.back());

  /*
  5   3  4  5   3

  2   0  1  2   0
  5   3  4  5   3

  2   0  1  2   0
  */

  CHECK(test.onLeftEdge(0));
  CHECK(test.onLeftEdge(3));
  CHECK(!test.onLeftEdge(1));
  CHECK(!test.onLeftEdge(4));
  CHECK(!test.onLeftEdge(2));
  CHECK(!test.onLeftEdge(5));

  CHECK(test.onUpperEdge(0));
  CHECK(test.onUpperEdge(1));
  CHECK(test.onUpperEdge(2));
  CHECK(!test.onUpperEdge(3));
  CHECK(!test.onUpperEdge(4));
  CHECK(!test.onUpperEdge(5));

  CHECK(test.onRightEdge(2));
  CHECK(test.onRightEdge(5));
  CHECK(!test.onRightEdge(0));
  CHECK(!test.onRightEdge(1));
  CHECK(!test.onRightEdge(3));
  CHECK(!test.onRightEdge(4));

  CHECK(test.onLowerEdge(3));
  CHECK(test.onLowerEdge(4));
  CHECK(test.onLowerEdge(5));
  CHECK(!test.onLowerEdge(0));
  CHECK(!test.onLowerEdge(1));
  CHECK(!test.onLowerEdge(2));

  CHECK(test.valueUpperRight(2) == 3);
  CHECK(test.valueUp(        2) == 5);
  CHECK(test.valueRight(     2) == 0);
  
  CHECK(test.valueLowerRight(5) == 0);
  CHECK(test.valueLow(       5) == 2);
  CHECK(test.valueRight(     5) == 3);

  CHECK(test.valueLowerLeft(3) == 2);
  CHECK(test.valueLow(      3) == 0);
  CHECK(test.valueLeft(     3) == 5);

  CHECK(test.valueUpperLeft(0) == 5);
  CHECK(test.valueUp(       0) == 3);
  CHECK(test.valueLeft(     0) == 2);
}

TEST_CASE("test 3x3 grid positions") {
  GameOfLife test;
  test.w = 3;
  test.h = 3;
  test.grid = { 0, 1, 2, 3, 4, 5, 6, 7, 8 };
  CHECK(test.grid.size() == test.w*test.h);
  CHECK(test.grid[test.grid.size()-1] == test.grid.back());

  /*
  8   6  7  8   6

  2   0  1  2   0
  5   3  4  5   3
  8   6  7  8   6

  2   0  1  2   0
  */

  CHECK(test.valueUp(0) == 6);
  CHECK(test.valueUp(1) == 7);
  CHECK(test.valueUp(2) == 8);
  CHECK(test.valueUp(3) == 0);
  CHECK(test.valueUp(4) == 1);
  CHECK(test.valueUp(5) == 2);
  CHECK(test.valueUp(6) == 3);
  CHECK(test.valueUp(7) == 4);
  CHECK(test.valueUp(8) == 5);

  CHECK(test.valueUpperRight(0) == 7);
  CHECK(test.valueUpperRight(1) == 8);
  CHECK(test.valueUpperRight(2) == 6);
  CHECK(test.valueUpperRight(3) == 1);
  CHECK(test.valueUpperRight(4) == 2);
  CHECK(test.valueUpperRight(5) == 0);
  CHECK(test.valueUpperRight(6) == 4);
  CHECK(test.valueUpperRight(7) == 5);
  CHECK(test.valueUpperRight(8) == 3);

  CHECK(test.valueRight(0) == 1);
  CHECK(test.valueRight(1) == 2);
  CHECK(test.valueRight(2) == 0);
  CHECK(test.valueRight(3) == 4);
  CHECK(test.valueRight(4) == 5);
  CHECK(test.valueRight(5) == 3);
  CHECK(test.valueRight(6) == 7);
  CHECK(test.valueRight(7) == 8);
  CHECK(test.valueRight(8) == 6);

  CHECK(test.valueLowerRight(0) == 4);
  CHECK(test.valueLowerRight(1) == 5);
  CHECK(test.valueLowerRight(2) == 3);
  CHECK(test.valueLowerRight(3) == 7);
  CHECK(test.valueLowerRight(4) == 8);
  CHECK(test.valueLowerRight(5) == 6);
  CHECK(test.valueLowerRight(6) == 1);
  CHECK(test.valueLowerRight(7) == 2);
  CHECK(test.valueLowerRight(8) == 0);

  CHECK(test.valueLow(0) == 3);
  CHECK(test.valueLow(1) == 4);
  CHECK(test.valueLow(2) == 5);
  CHECK(test.valueLow(3) == 6);
  CHECK(test.valueLow(4) == 7);
  CHECK(test.valueLow(5) == 8);
  CHECK(test.valueLow(6) == 0);
  CHECK(test.valueLow(7) == 1);
  CHECK(test.valueLow(8) == 2);

  CHECK(test.valueLowerLeft(0) == 5);
  CHECK(test.valueLowerLeft(1) == 3);
  CHECK(test.valueLowerLeft(2) == 4);
  CHECK(test.valueLowerLeft(3) == 8);
  CHECK(test.valueLowerLeft(4) == 6);
  CHECK(test.valueLowerLeft(5) == 7);
  CHECK(test.valueLowerLeft(6) == 2);
  CHECK(test.valueLowerLeft(7) == 0);
  CHECK(test.valueLowerLeft(8) == 1);

  CHECK(test.valueLeft(0) == 2);
  CHECK(test.valueLeft(1) == 0);
  CHECK(test.valueLeft(2) == 1);
  CHECK(test.valueLeft(3) == 5);
  CHECK(test.valueLeft(4) == 3);
  CHECK(test.valueLeft(5) == 4);
  CHECK(test.valueLeft(6) == 8);
  CHECK(test.valueLeft(7) == 6);
  CHECK(test.valueLeft(8) == 7);

  CHECK(test.valueUpperLeft(0) == 8);
  CHECK(test.valueUpperLeft(1) == 6);
  CHECK(test.valueUpperLeft(2) == 7);
  CHECK(test.valueUpperLeft(3) == 2);
  CHECK(test.valueUpperLeft(4) == 0);
  CHECK(test.valueUpperLeft(5) == 1);
  CHECK(test.valueUpperLeft(6) == 5);
  CHECK(test.valueUpperLeft(7) == 3);
  CHECK(test.valueUpperLeft(8) == 4);

  CHECK(test.onLeftEdge(0) == 1);
  CHECK(test.onLeftEdge(1) == 0);
  CHECK(test.onLeftEdge(2) == 0);
  CHECK(test.onLeftEdge(3) == 1);
  CHECK(test.onLeftEdge(4) == 0);
  CHECK(test.onLeftEdge(5) == 0);
  CHECK(test.onLeftEdge(6) == 1);
  CHECK(test.onLeftEdge(7) == 0);
  CHECK(test.onLeftEdge(8) == 0);

  CHECK(test.onUpperEdge(0) == 1);
  CHECK(test.onUpperEdge(1) == 1);
  CHECK(test.onUpperEdge(2) == 1);
  CHECK(test.onUpperEdge(3) == 0);
  CHECK(test.onUpperEdge(4) == 0);
  CHECK(test.onUpperEdge(5) == 0);
  CHECK(test.onUpperEdge(6) == 0);
  CHECK(test.onUpperEdge(7) == 0);
  CHECK(test.onUpperEdge(8) == 0);

  CHECK(test.onRightEdge(0) == 0);
  CHECK(test.onRightEdge(1) == 0);
  CHECK(test.onRightEdge(2) == 1);
  CHECK(test.onRightEdge(3) == 0);
  CHECK(test.onRightEdge(4) == 0);
  CHECK(test.onRightEdge(5) == 1);
  CHECK(test.onRightEdge(6) == 0);
  CHECK(test.onRightEdge(7) == 0);
  CHECK(test.onRightEdge(8) == 1);

  CHECK(test.onLowerEdge(0) == 0);
  CHECK(test.onLowerEdge(1) == 0);
  CHECK(test.onLowerEdge(2) == 0);
  CHECK(test.onLowerEdge(3) == 0);
  CHECK(test.onLowerEdge(4) == 0);
  CHECK(test.onLowerEdge(5) == 0);
  CHECK(test.onLowerEdge(6) == 1);
  CHECK(test.onLowerEdge(7) == 1);
  CHECK(test.onLowerEdge(8) == 1);
}

#endif
