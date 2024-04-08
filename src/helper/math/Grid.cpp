#include "Grid.h"
#include "RegTexUpdater.h"

Grid::Grid(SDLHandler* sh, TileMap* tm, Camera* cam, int64_t loadDistH, int stateToUse)
{
    //Initialize grid
    w = loadDistH*2+1;          //Grid is a (loadDistH*2+1)^2 square.
    h = w;
    alloc(w, w);                //Allocate this grid.

    /*
        Player should be at the center (0), which is @ grid[loadDistH][loadDistH]
        Generate array similar to the one below, which is 'w'*'w' large:
    
        4 3 2 3 4
        3 2 1 2 3
        2 1 0 1 2
        3 2 1 2 3
        4 3 2 3 4

        Additionally, for regions that are null, offscreen, or whose state is > 'stateToUse', their grid value is set to 1000.
    */
    for(int x = 0; x<w; x++) {
        for(int y = 0; y<h; y++) {
            grid[x][y] = taxiDist(x, y, loadDistH, loadDistH);    //Taxicab distance from center
            //If TileRegion loaded and regtexstate is > stateToUse, set grid[x][y] = 1000
            int64_t rX = cam->getRX()-loadDistH+x;
            int64_t rY = cam->getRY()-loadDistH+y;
            int64_t rZ = cam->getRZ();
            TileRegion* tr = tm->getRegByRXYZ(rX, rY, rZ);
            if(tr==nullptr || tr->getRegTexState()>stateToUse || !RegTexUpdater::isRegOnScreen(sh, cam, rX, rY, rZ) ) {
                grid[x][y] = 1000;
            }
        }
    }
}

Grid::~Grid()
{

}

void Grid::logInfo()
{
    std::cout << "(w, h) = (" << w << ", " << h << ")\n";
    for(int x = 0; x<w; x++) {
        for(int y = 0; y<h; y++) {
            if(grid[x][y]<10) {
                std::cout << grid[x][y] << " ";
            } else if(grid[x][y]==1000) {
                std::cout << "x ";
            } else {
                std::cout << ". ";
            }
        }
        std::cout << "\n";
    }
}

/**
    For a wxh=32x32 array: x and y must be from 0 to 31.
*/
void Grid::floodfill(int** array, int w, int h, int x, int y, int c)
{
    if( x<0||x>=w || y<0||y>=h ) return;

    int lastColor = array[x][y];

    array[x][y] = c;
}



int Grid::taxiDist(int x1, int y1, int x2, int y2) { return std::abs(y2-y1)+std::abs(x2-x1); }

std::pair<int, int> Grid::getLowestElementXY()
{
    if(w==0 || h==0) {
        return std::make_pair(-1, -1);
    }

    int leX = 0; int leY = 0;   //(L)east (E)lement X/Y
    for(int x = 0; x<w; x++) {
        for(int y = 0; y<h; y++) {
            if(grid[x][y]<grid[leX][leY]) {
                leX = x;
                leY = y;
            }
        }
    }

    if(grid[leX][leY]==1000) {
        return std::make_pair(-1, -1);
    }

    return std::make_pair(leX, leY);
}


void Grid::alloc(int w, int h)
{
    grid = (int**)malloc(w*sizeof(*grid));
    for (int i = 0; i<w; i++) {
        grid[i] = (int*)malloc(h*sizeof(*grid[i]));
    }
}