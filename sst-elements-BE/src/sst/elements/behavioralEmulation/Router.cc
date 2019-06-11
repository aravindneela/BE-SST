#include "Router.h"
#include <vector>
#include <math.h>
#include <algorithm>
#include <stdlib.h>
#include <iostream>

using namespace SST;
using namespace SST::BEComponent;


int DynamicRouter::peerTraversalForMesh(int target)
{    
    int weightedSource = ordinal;
    int weightedTarget = target; 

    for(int j = containerDimensions.size()-1; j >= 0; j--)
    {
        if(weightedSource % (containerDimensions[j]) < weightedTarget % (containerDimensions[j])) return plusNeighbour[j];

        else if(weightedSource % (containerDimensions[j]) > weightedTarget % (containerDimensions[j])) return minusNeighbour[j];

        else {
            weightedSource = weightedSource/containerDimensions[j];
            weightedTarget = weightedTarget/containerDimensions[j];
        }
    }

    return -1;

}

int DynamicRouter::peerTraversalForTorus(int target)
{
    int weightedSource = ordinal;
    int weightedTarget = target;

    for(int j = containerDimensions.size()-1; j >= 0; j--)
    {
        int srcPos = weightedSource % (containerDimensions[j]);
        int tarPos = weightedTarget % (containerDimensions[j]); 

        if((srcPos < tarPos) && (tarPos-srcPos < containerDimensions[j]-tarPos+srcPos)) return plusNeighbour[j];

        else if(srcPos < tarPos) return minusNeighbour[j];

        else if(srcPos > tarPos && (srcPos-tarPos < containerDimensions[j]-srcPos+tarPos)) return minusNeighbour[j];

        else if(srcPos > tarPos) return plusNeighbour[j];

        else {
            weightedSource = weightedSource/containerDimensions[j];
            weightedTarget = weightedTarget/containerDimensions[j];
        }
    }

    return -1;

}

std::tuple<int, int, int> DynamicRouter::findNextHopWC(int target, int targetOneLevelDown)
{
    std::map<int, std::tuple<int, int, int>>::iterator it;
    it = cache.find(target);

    if(it != cache.end())
    {
        return it->second;
    }
    else
    {
        std::tuple<int, int, int> nh = std::make_tuple(0, -1, -1);

        /* if true, we need to route the message one level above to the parent container */
        if((ordinal/containerSize) != (target/containerSize)) {
            nh = std::make_tuple(1, parent, target/containerSize);
            cache[target] = nh;
            return nh;
        }

        /* if true, we need to route the message one level down to the appropriate child object */
        if(target == ordinal) {
            int offset = targetOneLevelDown % childrenList.size();
            nh = std::make_tuple(-1, childrenList[offset], target);
            cache[target] = nh;
            return nh;
        }

        if(topology == "mesh") nh = std::make_tuple(0, peerTraversalForMesh(target), target);

        else if(topology == "torus") nh = std::make_tuple(0, peerTraversalForTorus(target), target);

        cache[target] = nh;
        return nh;
    }

    return std::make_tuple(0, -1, -1);
}

std::tuple<int, int, int> DynamicRouter::findNextHop(int target, int targetOneLevelDown)
{
    std::tuple<int, int, int> nh = std::make_tuple(0, -1, -1);

    /* if true, we need to route the message one level above to the parent container */
    if((ordinal/containerSize) != (target/containerSize)) {
        nh = std::make_tuple(1, parent, target/containerSize);
        return nh;
    }

    /* if true, we need to route the message one level down to the appropriate child object */
    if(target == ordinal) {
        int offset = targetOneLevelDown % childrenList.size();
        nh = std::make_tuple(-1, childrenList[offset], target);
        return nh;
    }

    if(topology == "mesh") nh = std::make_tuple(0, peerTraversalForMesh(target), target);

    else if(topology == "torus") nh = std::make_tuple(0, peerTraversalForTorus(target), target);

    return nh;
}


std::vector<std::vector<int>> Router::multiply (std::vector<std::vector<int>>a, int b){
    std::vector<std::vector<int>> mult;
    for (int i = 0; i < b; i++){
        for (std::vector<std::vector<int>>::iterator itr = a.begin(); itr != a.end(); itr++)
            mult.push_back(*itr);
    }

    return mult;
}

std::vector<std::vector<int> > Router::ident(int n, int x){
    std::vector<std::vector<int> > my_vec;
    for (int i = 0; i < n; i++){
        std::vector<int> subvector;
        //my_vec.push_back(std::vector<int>());
        for (int j = 0 ; j < n; j++){
            if (i == j)
                subvector.push_back((int)copysign(1,x));              
            else
                subvector.push_back(0);
        } 
        my_vec.push_back(subvector);  
    }
    return my_vec;
}

std::vector<int> Router::add(std::vector<int> a, std::vector<int> b){
    std::vector<int> sum;
    std::vector<int>::size_type sz;
    
    /*if (a.size() <= b.size())
        sz = a.size();      
    else
        sz = b.size();*/
    sz = std::min(a.size(), b.size());

    for (unsigned i = 0; i < sz; i++){
        sum.push_back(a[i] + b[i]);
    }

    return sum;
}

std::vector<int> Router::sub(std::vector<int> a, std::vector<int> b){
    std::vector<int> diff;
    std::vector<int>::size_type sz;
    
    /*if (a.size() <= b.size())
        sz = a.size();      
    else
        sz = b.size();*/
    sz = std::min(a.size(), b.size());
    
    for (unsigned i = 0; i < a.size(); i++){
        //std::cout << a[i] << " " << b[i] << std::endl;
        diff.push_back(b[i] - a[i]);
    }

    return diff;
}

std::vector<int> Router::torusAdd(std::vector<int> a, std::vector<int> b, std::vector<int> sizes){
    std::vector<int> sum;
    std::vector<int>::size_type sz;
    
    sz = std::min(a.size(), b.size());
    sz = std::min(sz, sizes.size());

    for (unsigned i = 0; i < sz; i++){
        if((a[i]+b[i]) < 0)
            sum.push_back(sizes[i] + ((a[i] + b[i]) % sizes[i]));
        else      
            sum.push_back((a[i] + b[i]) % sizes[i]);
    }
    
    return sum;    
}

std::vector<int> Router::torusSub(std::vector<int> a, std::vector<int> b, std::vector<int> sizes){
    std::vector<int> torus_diff;
    std::vector<int> diff = sub(a, b);
    std::vector<int>::size_type sz = diff.size();      

    for (unsigned i = 0; i < sz; i++){
        if (abs(diff[i]) <= sizes[i]/2) 
            torus_diff.push_back(diff[i]);
        else
            torus_diff.push_back((int)copysign(abs(diff[i]) - sizes[i], -diff[i]));
    } 

    return torus_diff;  
}

std::vector<std::vector<std::vector<int>> > Router::deltas(std::vector<int> a, std::vector<int> b){
    std::vector<std::vector<std::vector<int>> > deltas;    
    std::vector<int> diff = sub(a,b);    
    std::vector<int>::size_type sz = diff.size();

    for (unsigned i = 0; i < sz; i++){        
        std::vector<std::vector<int>> ident_v = ident(a.size(), diff[i]);
        std::vector<std::vector<int>> list_idents;
        list_idents.push_back(ident_v[i]);
        deltas.push_back(multiply(list_idents, abs(diff[i])));
    }

    return deltas;
}

std::vector<std::vector<std::vector<int>> > Router::torusDeltas(std::vector<int> a, std::vector<int> b, std::vector<int> sizes){
    std::vector<std::vector<std::vector<int>> > deltas;
    std::vector<int> diff = torusSub(a, b, sizes);
    std::vector<int>::size_type sz = diff.size();

    for (unsigned i = 0; i < sz; i++){
        std::vector<std::vector<int>> ident_v = ident(a.size(), diff[i]);
        std::vector<std::vector<int>> list_idents;
        list_idents.push_back(ident_v[i]);
        deltas.push_back(multiply(list_idents, abs(diff[i])));
    }
    
    return deltas;
}   


void Router::clear(){

    std::vector<int> forward_emp, reverse_emp;
    std::vector<std::vector<int> > across_emp;

    std::swap(forward, forward_emp);
    std::swap(reverse, reverse_emp);
    std::swap(across, across_emp);     

}

void Router::display(std::vector<int> l){

    for(int i=0; i<l.size(); i++) std::cout<<l[i]<<", ";
    std::cout<<"\n";
}

void Router::prep(int source, int target){
    
    int scope;   

    scope = layout->parents[layout->ordinals[source]];
    std::vector<int>::iterator it; 

    if(scope == -1) std::cout<<"ERROR: Invalid source!\n"; // raise error;   

    it = std::find(layout->subordinals[scope].begin(), layout->subordinals[scope].end(), target);

    while (it == layout->subordinals[scope].end()){   //check: time complexity
        scope = layout->parents[scope];  
        it = std::find(layout->subordinals[scope].begin(), layout->subordinals[scope].end(), target); 
    }

    forward.push_back(layout->ordinals[source]);
    reverse.push_back(layout->ordinals[target]);   

    it = std::find(layout->children[scope].begin(), layout->children[scope].end(), forward.back());
    while (it == layout->children[scope].end()){        
        forward.push_back(layout->parents[forward.back()]);
        it = std::find(layout->children[scope].begin(), layout->children[scope].end(), forward.back());
    }

    it = std::find(layout->children[scope].begin(), layout->children[scope].end(), reverse.back());
    while (it == layout->children[scope].end()){        
        reverse.push_back(layout->parents[reverse.back()]);
        it = std::find(layout->children[scope].begin(), layout->children[scope].end(), reverse.back());
    }

    netname = layout->netnames[forward.back()];
    
    netsize = layout->netsizes[forward.back()]; 
    //std::cout<<"netname "<<netname<<"\n";       
}


std::vector<int> Router::path(int source, int target){
    //std::cout << source << " " << target << std::endl;
    int maxordinal = layout->ordinals.size();
    
    if (source < 0)
        source = source + maxordinal;

    if (source >= maxordinal)
        source = source - maxordinal;
    
    if (target < 0)
        target = target + maxordinal;

    if (target >= maxordinal)
        target = target - maxordinal;
      
    if (layout->ordinals.end() == layout->ordinals.find(source) || layout->ordinals.end() == layout->ordinals.find(target) || source == target){
        std::cout << source << " " << target << " "<< "\n";
        std::cout << "Error" << std::endl;    
     }    

    //std::cout << "here2" << std::endl;
    prep(source, target);
    /*for (std::vector<int>::iterator itr = forward.begin(); itr < forward.end(); itr++){
        //std::cout << *itr << " ";
    }
    //std::cout << std::endl;
    for (std::vector<int>::iterator itr = reverse.begin(); itr < reverse.end(); itr++){
        //std::cout << *itr << " ";
    }
    //std::cout << std::endl;
    for (std::vector<int>::iterator itr = netsize.begin(); itr < netsize.end(); itr++){
        //std::cout << *itr << " ";
    }
    //std::cout << std::endl;*/
    //std::cout << "here3" << std::endl;

    std::vector<int> start, end;

    //std::cout <<"Forward is: \n";
    //display(forward);
    //std::cout <<"Reverse is: \n";
    //display(reverse);

    start = layout->indices[forward.back()];
    end = layout->indices[reverse.back()];
    
    //std::cout <<"Start is: \n";
    //display(start);
    //std::cout <<"End is: \n";
    //display(end);
    
    std::vector<std::vector<std::vector<int>> > dels;
    
    if (netname == "mesh" || netname == "cube" || netname == "torus"){
        if (netname == "mesh" || netname == "cube"){
            dels = deltas(start, end);             
        }
        else if (netname == "torus"){
            dels = torusDeltas(start, end, netsize);
        }
        //std::cout << "here4" << std::endl;    
        if (policies[netname] == "first-to-last")        
            dels = dels;
        else if (policies[netname] == "last-to-first")
            std::reverse(dels.begin(), dels.end());
        else if (policies[netname] == "shorter-to-longer")
            std::sort(dels.begin(), dels.end(), [](const std::vector<std::vector<int>> & a, const std::vector<std::vector<int>> & b){ return a.size() < b.size(); });
        else if (policies[netname] == "longer-to-shorter")
            std::sort(dels.begin(), dels.end(), [](const std::vector<std::vector<int>> & a, const std::vector<std::vector<int>> & b){ return a.size() > b.size(); });    
        

        across.push_back(start);
        //std::cout<<"First element of across:-\n";
        //display(start);
        for (std::vector<std::vector<std::vector<int>>>::iterator dir = dels.begin(); dir != dels.end(); dir++){
            for (std::vector<std::vector<int> >::iterator itr = dir->begin(); itr != dir->end(); itr++){
                if (netname == "mesh" || netname == "cube"){
                    across.push_back(add(across.back(), *itr));
                    //std::cout<<"next element of across:-\n";
                    //display(add(across.back(), *itr));                
                }
                else{
                    //std::cout<<"Deltas is:-\n";
                    //display(*itr);
                    across.push_back(torusAdd(across.back(), *itr, netsize));
                    
                }            
            }
        } 
 
        //std::cout<<" Across is "<<"\n";
        //for (std::vector<std::vector<int> >::iterator itr = across.begin(); itr != across.end(); itr++){
            //for(std::vector<int>::iterator itr1 = itr->begin(); itr1 != itr->end(); itr1++){
                //std::cout << *itr1 << ", ";
            //}
            //std::cout << std::endl;
        //}
        //std::cout << "here5" << std::endl;           
    }
    else if (netname == "tree"){
        std::vector<std::vector<int>> ascent, descent;
        ascent.push_back(start);
        descent.push_back(end);
        std::vector<int> temp;

        while (ascent.back() != descent.back()){
            std::vector<std::vector<int>> ascent_temp = ascent, descent_temp = descent;
            ascent_temp.back().pop_back();        
            ascent.push_back(ascent_temp.back());

            descent_temp.back().pop_back();
            descent.push_back(descent_temp.back());
            //std::cout<<"Ascent is: \n";
            //for(auto itr = ascent.begin(); itr != ascent.end(); itr++) display(*itr);
            //std::cout<<"Descent is: \n";
            //for(auto itr = descent.begin(); itr != descent.end(); itr++) display(*itr);

        }
        
        std::reverse(descent.begin(), descent.end());

        across.insert(across.begin(), ascent.begin(), ascent.end());
        across.insert(across.end(), descent.begin() + 1, descent.end());

        //std::cout<<"Across is "<<"\n";
        //for(auto itr = across.begin(); itr != across.end(); itr++) display(*itr);
    }

    std::vector<std::vector<int>> edge;

    for (std::vector<std::vector<int> >::iterator itr = across.begin() + 1; itr != across.end(); itr++){
        edge = layout->edges[forward.back()];  
        //std::cout<<" Across is "<<(*itr)[0]<<", "<<(*itr)[1]<<"\n";      
        //std::cout << "outer itr " << std::endl;
        for (unsigned i = 0; i < edge.size(); i++){
            //std::cout << "inner itr " <<i<<" "<<edge[i][0]<<std::endl;
            //std::cout<<" Indices is "<<layout->indices[edge[i][0]][0]<<", "<<layout->indices[edge[i][0]][1]<<"\n";
            if (layout->indices[edge[i][0]] == *itr){
                //printf("condition satisfied\n");
                forward.push_back(edge[i][1]);
                forward.push_back(edge[i][0]);
                //std::cout <<"Forward is: \n";
                //display(forward);
                break;
            }
        }    
    }

    std::reverse(reverse.begin(), reverse.end());
    //std::cout <<"Forward is: \n";
    //display(forward);
    //std::cout <<"Reverse is: \n";
    //display(reverse);
    
    forward.insert(forward.end(), reverse.begin() + 1, reverse.end());  
    //std::cout <<"Forward is: \n";
    //display(forward);
    //std::cout <<"Reverse is: \n";
    //display(reverse);
  
    return forward;
    /*for (std::vector<int>::iterator itr = forward.begin(); itr < forward.end(); itr++){
        //std::cout << *itr << " " << std::endl;
    }*/
}
