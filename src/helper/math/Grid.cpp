#include "Grid.h"
#include "RegTexInfo.h"


Grid::~Grid()
{
    delete grid;
}

void Grid::updateOrderFinder(SDLHandler* sh, TileMap* tm, Camera* cam, int loadDist, int stateToUse)
{
    //Initialize grid
    w = loadDist*2+1;         //Grid is a (squaresize*2+1)^2 square.
    h = w;
    alloc(w, h);                //Allocate this grid.

    /*
        Player should be at the center (0), which is @ grid[squaresize][squaresize]
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

            //Set all grid squares to be the taxidist from the center
            grid[x][y] = taxiDist(x, y, loadDist, loadDist);    //Taxicab distance from center
            
            //Set certain grid squares to be 1000 (ignored) depending on the situation
            int64_t csRX = cam->getCsRX()-loadDist+x;
            int64_t csRY = cam->getCsRY()-loadDist+y;
            int64_t csRZ = cam->getCsRZ();

            bool completePillar = true;
            for(int64_t icsRZ = csRZ-loadDist; icsRZ<=csRZ+loadDist; icsRZ++) {
                if(stateToUse!=-1) {
                    TileRegion* tr = tm->getRegByCsRXYZ(cam, csRX, csRY, icsRZ);
                    if(tr==nullptr || tr->getRegTexState()>stateToUse || !RegTexInfo::isCsRegOnScreen(cam, csRX, csRY) ) {
                        grid[x][y] = 1000;
                    }
                } else {
                    TileRegion* tr = tm->getRegByCsRXYZ(cam, csRX, csRY, icsRZ);
                    if(tr==nullptr ) {     
                        completePillar = false;
                    }
                }
            }
            
            if(completePillar && stateToUse==-1) {
                grid[x][y] = 1000;
            }
        }
    }
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