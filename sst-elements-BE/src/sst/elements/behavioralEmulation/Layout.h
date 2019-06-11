#ifndef LAYOUT_H
#define LAYOUT_H

#include <vector>
#include <iostream>
#include <map>
#include <string>

namespace SST{
namespace BEComponent{

class Layout{
public:
    Layout(){

  /*  	cid.push_back(0);
    	cid.push_back(0);
    	cid.push_back(1);
    	cid.push_back(0);

    	kinds.push_back("node");
    	kinds.push_back("xeon-e5-core");
    	kinds.push_back("xeon-e5-core");
    	kinds.push_back("qpi");

        std::vector<int> el1, el2, emel;
        el1.push_back(2);
        el1.push_back(3);
        el2.push_back(1);
        el2.push_back(3);
        edges.push_back(emel);
        edges.push_back(el1);
        edges.push_back(el2);
        edges.push_back(emel);

  /*      parents.push_back(-1);
        parents.push_back(0);
        parents.push_back(0);
        parents.push_back(0);
  */     
        /*std::vector<int> cl, ecl;
        cl.push_back(1);
        cl.push_back(2);
        cl.push_back(3);
        children.push_back(cl);
        children.push_back(ecl);
        children.push_back(ecl);
        children.push_back(ecl);

        std::vector<int> il1, il2;
        il1.push_back(0);
        il2.push_back(1);
        indices[1] = il1;
        indices[2] = il2;

/*      netnames[1] = "mesh";
        netnames[2] = "mesh";
        netnames[3] = "mesh";


        std::vector<int> nsl;
        nsl.push_back(2);
        netsizes[1] = nsl;
        netsizes[2] = nsl;
        netsizes[3] = nsl;

/*      rordinals[1] = 0;
        rordinals[2] = 1;

        ordinals[0] = 1;
        ordinals[1] = 2;

        std::vector<int> sl;
        sl.push_back(0);
        sl.push_back(1);
        subordinals[0] = sl;
    	
    	std::map<std::string, int> d1, d2;
    	d1["cpu"] = 1;
    	d2["cpu"] = 2;
    	relations[1] = d1;
    	relations[2] = d2;*/

    	
    }

    std::vector<int> cid;
    std::vector<std::string> kinds;
    std::vector<std::vector<std::vector<int>> > edges; // change here and in router
    std::vector<int> parents;
    std::vector<std::vector<int> > children;
    
    std::map<int, std::vector<int> > indices;
    std::map<int, std::string> netnames;
    std::map<int, std::vector<int> > netsizes;
    std::map<int, std::map<std::string, int> > relations;
    std::map<int, int> ordinals;
    std::map<int, int> rordinals;
    std::map<int, std::vector<int> > subordinals;
    //tallies

    void build(std::string file);

};
}
}

#endif
