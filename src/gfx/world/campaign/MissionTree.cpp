#include "MissionTree.h"
#include <nch/cpp-utils/log.h>
#include <nch/cpp-utils/timer.h>

MissionTree::MissionTree(){}
MissionTree::~MissionTree(){}

void MissionTree::build(std::map<std::string, std::vector<Mission>>& missions, std::string tabID)
{
    nch::Timer t;

    //Get list of missions from the provided 'tabID'.
    auto itr = missions.find(tabID);
    if(itr==missions.end()) {
        nch::Log::log("Couldn't find any tabs with the ID \"%s\"", tabID.c_str());
        return;
    }
    std::vector<Mission>* pTab = &itr->second;

    //Add mission nodes with only their ID and parent set
    for(int i = 0; i<pTab->size(); i++) {
        Node* n = new Node();
        n->id = pTab->at(i).id;
        n->parent = pTab->at(i).parent;
        missionTree.push_back(n);
    }

    //Set other properties of mission nodes
    for(int i = 0; i<missionTree.size(); i++) {
        Node* n = missionTree[i];
        n->layer = getNodeDepth(n);
        n->children = getNodeChildren(n);

        n->familyBox.x = 0;
        n->familyBox.y = n->layer*2;
        n->familyBox.w = 1;
        n->familyBox.h = 1;
    }

    //Update nodes by layer (max -> 0)
    for(int j = 0; j<1; j++)
    for(int i = getMaxNodeLayer(); i>=0; i--) {
        //updateNodesByLayer(i);
    }

    nch::Log::log("Built tree containing %d missions and %d layers in %fms.", missionTree.size(), getMaxNodeLayer(), t.getElapsedTimeMS());
}

void MissionTree::draw(SDLHandler* sh, int x, int y)
{
    bool dbg = false;

    SDL_Renderer* rend = sh->getRenderer();

    int tx = 100+x;
    int ty = 100+y;

    for(int i = 0; i<missionTree.size(); i++) {
        Node* n = missionTree[i];
        double s = 8;

        //Draw lines from this parent mission to its child mission(s).
        for(int j = 0; j<n->children.size(); j++) {
            Node* nc = n->children[j];

            int x0 = getFinalNodeBox(n->familyBox, s).x+s+tx;
            int y0 = getFinalNodeBox(n->familyBox, s).y+2*s+ty;
            int x1 = getFinalNodeBox(nc->familyBox, s).x+s+tx;
            int y1 = getFinalNodeBox(nc->familyBox, s).y+ty;

            sh->setRenderDrawColor(nch::Color(0, 0, 255));
            SDL_RenderDrawLine(rend, x0, y0, x0, (y1+y0)/2);
            SDL_RenderDrawLine(rend, x0, (y1+y0)/2, x1, (y1+y0)/2);
            SDL_RenderDrawLine(rend, x1, (y1+y0)/2, x1, y1);
        }

        sh->setRenderDrawColor(nch::Color(255, 0, 0));
        SDL_Rect scaledRect = getFinalNodeBox(n->familyBox, s);
        if(dbg) {
            scaledRect.x = n->familyBox.x*s;
            scaledRect.y = n->familyBox.y*2*s;
            scaledRect.w = n->familyBox.w*s;
            scaledRect.h = n->familyBox.h*s;
        }
        scaledRect.x += tx;
        scaledRect.y += ty;
        SDL_RenderDrawRect(sh->getRenderer(), &scaledRect);
        sh->setRenderDrawColor(nch::Color(0, 0, 0));
    }

    int mnl = getMaxNodeLayer();
    if(mnl-test<0) return;
    updateNodesByLayer(mnl-test);
    test++;
}






MissionTree::Node* MissionTree::getNodeByID(std::string id)
{
    for(int i = 0; i<missionTree.size(); i++)
        if(missionTree[i]->id==id)
            return missionTree[i];
    return nullptr;
}

MissionTree::Node* MissionTree::getNodeParent(Node* n)
{
    if(n->parent=="ROOT") return nullptr;
    return getNodeByID(n->parent);
}

std::vector<MissionTree::Node*> MissionTree::getNodeChildren(Node* n)
{
    std::vector<Node*> res;
    for(int i = 0; i<missionTree.size(); i++)
        if(missionTree[i]->parent==n->id)
            res.push_back(missionTree[i]);
    return res;
}

std::vector<MissionTree::Node*> MissionTree::getNodeAndDescendants(Node* n)
{
    std::vector<Node*> res;
    if(n==nullptr) return res;
    
    //Add the specified element ('mn')
    res.push_back(n);

    //Don't continue if no children
    if(n->children.size()==0) {
        return res;
    }

    //Add 'n's children and their descendants
    for(int i = 0; i<n->children.size(); i++) {
        auto childTree = getNodeAndDescendants(n->children[i]);
        for(int j = 0; j<childTree.size(); j++) {
            if(childTree[j]!=nullptr)
                res.push_back(childTree[j]);
        }
    }

    //Return
    return res;
}

std::vector<MissionTree::Node*> MissionTree::getNodesInLayer(int layer)
{
    std::vector<Node*> res;
    for(int i = 0; i<missionTree.size(); i++)
        if(missionTree[i]->layer==layer)
            res.push_back(missionTree[i]);
    return res;
}

int MissionTree::getMaxNodeLayer()
{
    int maxLayer = -1;
    for(int i = 0; i<missionTree.size(); i++)
        if(missionTree[i]->layer>maxLayer)
            maxLayer = missionTree[i]->layer;
    return maxLayer;
}

int MissionTree::getNodeDepth(Node* mn, int layers)
{
    if(layers>999) {
        nch::Log::warnv( __PRETTY_FUNCTION__, "returning -9999",
            "Maximum of 999 layers checked (due to loop or unreachable \"ROOT\"?) with missionID==%s",
            mn->id.c_str() );
    }

    if(mn->parent=="ROOT") {
        return layers;
    }

    layers++;
    return getNodeDepth(getNodeByID(mn->parent), layers);
}

int MissionTree::getNodeDepth(Node* n) { return getNodeDepth(n, 0); }

bool MissionTree::nodeExists(Node* n)
{
    for(int i = 0; i<missionTree.size(); i++) if(missionTree[i]==n) return true;
    return false;
}

SDL_Rect MissionTree::getFinalNodeBox(SDL_Rect finalFamilyBox, double scale)
{
    int cx = finalFamilyBox.x+finalFamilyBox.w/2;
    
    SDL_Rect res;
    res.x = scale*2*(cx-1);               res.w = scale*2;
    res.y = scale*2*(finalFamilyBox.y);   res.h = scale*2;
    return res;
}


/*
    Build a node's family box from its direct children
*/
void MissionTree::updateNodeFamilyBox(Node* mn)
{
    if(mn->children.size()==0) {
        return;
    }

    //Find minimum X that any child's family box occupies.
    int minFamX = mn->children[0]->familyBox.x;
    for(int i = 0; i<mn->children.size(); i++) {
        SDL_Rect cfb = mn->children[i]->familyBox;  //Child's family box
        if(cfb.x<minFamX) {
            minFamX = cfb.x;
        }
    }

    //Find maximum X that any child's family box occupies.
    int maxFamX = mn->children[0]->familyBox.x + mn->children[0]->familyBox.w;
    for(int i = 0; i<mn->children.size(); i++) {
        SDL_Rect cfb = mn->children[i]->familyBox;  //Child's family box
        if(cfb.x+cfb.w>maxFamX) {
            maxFamX = cfb.x+cfb.w;
        }
    }

    //Update family box for 'mn'.
    mn->familyBox.x = minFamX;
    mn->familyBox.w = maxFamX-minFamX;
}

void MissionTree::updateNodesByLayer(int layer)
{
    //Get all nodes within this 'layer'
    auto nil = getNodesInLayer(layer);
        
    //Build family boxes of all nodes on this 'layer', based on their direct children.
    for(int i = 0; i<nil.size(); i++) {
        Node* n = nil[i];
        updateNodeFamilyBox(n);
    }

    //Go through all nodes on this layer, find 'starts' and 'widths'
    int currSplitX = 0;
    std::vector<int> starts;
    std::vector<int> widths;

    for(int i = 0; i<nil.size(); i++) {
        //Find the horizontal size of this node group (descendants of nil[i])
        auto desc = getNodeAndDescendants(nil[i]);
        int minX = 9999999;
        int maxX = -9999999;
        for(int j = 0; j<desc.size(); j++) {
            if(desc[j]->familyBox.x<minX) {
                minX = desc[j]->familyBox.x;
            }
            if(desc[j]->familyBox.x+desc[j]->familyBox.w>maxX) {
                maxX = desc[j]->familyBox.x+desc[j]->familyBox.w;
            }
        }

        //Add split location
        starts.push_back(currSplitX);
        currSplitX += (maxX-minX);
    }
    starts.push_back(currSplitX);

    //Based on 'splits', move node groups around so that none will be colliding.
    for(int i = 0; i<starts.size()-1; i++) {
        auto desc = getNodeAndDescendants(nil[i]);
        int childrenMinX = 999999;
        for(int j = 0; j<desc.size(); j++) {
            if(desc[j]->familyBox.x<childrenMinX) {
                childrenMinX = desc[j]->familyBox.x;
            }
        }

        for(int j = 0; j<desc.size(); j++) {
            if(desc[j]!=nil[i]) {
                desc[j]->familyBox.x += (starts[i]-childrenMinX);
            } else {
                desc[j]->familyBox.x = starts[i];
                desc[j]->familyBox.w = starts[i+1]-starts[i];
            }
            
        }
    }

    if(layer==3) {
        int x = 4;
        x++;
        x++;
    }
}