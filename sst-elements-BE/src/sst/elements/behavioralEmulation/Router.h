#ifndef ROUTER_H
#define ROUTER_H

#include <vector>
#include <string>
#include <map>
#include "Layout.h"
namespace SST{
namespace BEComponent{


class DynamicRouter{
public:
    DynamicRouter(std::string topo, int o, int p, std::vector<int> cd, std::vector<int> pl, std::vector<int> ml, std::vector<int> cl) {
        topology = topo;
        ordinal = o;
        parent = p;
        containerDimensions = cd;
        plusNeighbour = pl;
        minusNeighbour = ml;
        childrenList = cl;
        containerSize = 1;

        for(int i=0; i<containerDimensions.size(); i++) containerSize = containerSize*containerDimensions[i];
    }

    std::tuple<int, int, int> findNextHop(int, int);
    std::tuple<int, int, int> findNextHopWC(int, int);
    int peerTraversalForMesh(int);
    int peerTraversalForTorus(int);

    ~DynamicRouter(){}

private:
    std::vector<int> containerDimensions, plusNeighbour, minusNeighbour, childrenList;
    int ordinal, parent, containerSize;
    std::string topology;
    std::map<int, std::tuple<int, int, int>> cache;
};

class Router{
public: 
    Router(Layout* l,  std::map<std::string, std::string> p){
        policies = p;
        layout = l;
    }

    std::vector<int> path(int src, int target);
    void clear();
    ~Router(){}

private:
    std::vector<std::vector<int>> multiply (std::vector<std::vector<int>> a, int b);        
    std::vector<std::vector<int>> ident(int n, int x);
    std::vector<int> add(std::vector<int> a, std::vector<int> b);
    std::vector<int> sub(std::vector<int> a, std::vector<int> b);
    std::vector<int> torusAdd(std::vector<int> a, std::vector<int> b, std::vector<int> sizes);
    std::vector<int> torusSub(std::vector<int> a, std::vector<int> b, std::vector<int> sizes);
    std::vector<std::vector<std::vector<int>> > deltas(std::vector<int> a, std::vector<int> b); 
    std::vector<std::vector<std::vector<int>> > torusDeltas(std::vector<int> a, std::vector<int> b, std::vector<int> sizes);
    void prep(int src, int target);  
    void display(std::vector<int> l);  

    std::vector<int> forward, reverse;
    std::string netname;
    std::vector<int> netsize;
    std::map<std::string, std::string> policies;
    std::vector<std::vector<int> > across;
    //std::multimap<int, int> cache;  
    Layout* layout;
};

}
}

#endif
