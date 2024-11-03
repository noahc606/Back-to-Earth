#pragma once
#include <string>
#include <vector>
#include "Mission.h"
#include "SDLHandler.h"


class MissionTree {
public:
    struct Node {
        int layer = -1;
        std::string id = "null";
        std::string parent = "null";
        std::vector<Node*> children;

        //"Placement zone" of this node and all of its descendants within the mission tree
        SDL_Rect familyBox;
        bool initialized = false;
    };

    MissionTree();
    ~MissionTree();

    void build(std::map<std::string, std::vector<Mission>>& missions, std::string tabID);
    void draw(SDLHandler* sh, int x, int y);
private:
    Node* getNodeByID(std::string id);
    Node* getNodeParent(Node* n);
    std::vector<Node*> getNodeChildren(Node* n);
    std::vector<Node*> getNodeAndDescendants(Node* n);
    std::vector<Node*> getNodesInLayer(int layer);
    int getMaxNodeLayer();
    int getNodeDepth(Node* n, int checks);
    int getNodeDepth(Node* n);
    bool nodeExists(Node* n);
    SDL_Rect getFinalNodeBox(SDL_Rect finalFamilyBox, double scale);

    void updateNodeFamilyBox(Node* n);
    void updateNodesByLayer(int layer);

    //Tree-diagram for the set of missions under the tab currently shown.
    std::vector<Node*> missionTree;
    int test = 0;
};