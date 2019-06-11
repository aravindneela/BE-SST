#ifndef ROUTINE_H
#define ROUTINE_H

#include <map>
#include <vector>
#include <queue>
#include <functional>
#include <memory>
#include "Sim_Events.h"
#include "Process_Events.h"
#include "Event_Template.h"
#include "Process.h"
#include <sst/core/event.h>
#include "Containers.h"
#include <boost/function.hpp>

namespace SST {
namespace BEComponent {

class eventTemplate;

class Routine : public Process{
public:
    explicit Routine(int g, int e, std::map<std::string, double> s, std::vector<float> inp, std::vector<double> out, std::queue<std::shared_ptr<eventTemplate>> seq, std::string cre) : Process(){
        gid = g;	
        eventId = e;
	state = s;          
        input = inp;       
        output = out;      
        sequence = seq;    
        type = "Routine";
        creator = cre;	//Instruction/operation that created the routine
        init_check();
        event_template = NULL;
        //printf("creating routine\n");
	}
    
    ~Routine(){};

    int gid, pid, eventId; // pid?
    std::string creator;

    std::shared_ptr<simEvent> run();

    std::tuple<std::vector<float>, std::vector<double>> getRoutineParams() {
        return std::tie(input, output);
    }


private:
    //std::map<std::string, double> state;
    std::vector<float> input;
    std::vector<double> output; 
    std::queue<std::shared_ptr<eventTemplate>> sequence;
    std::shared_ptr<eventTemplate> event_template;
    
    std::map<std::string, boost::function<bool(double, double)>> check;
	
	void init_check(){
		check["=="] = std::equal_to<double>();
		check["!="] = std::equal_to<double>();
		check["<="] = std::less_equal<double>();
		check[">="] = std::greater_equal<double>();
		check["<"] = std::less<double>();
		check[">"] = std::greater<double>();
	}
	
    double find(Procrastinator* value);
	
    //template <class T1, class T2> T2 find(T1 value);
    
};

//template <class T2> T2 Routine::find<Procrastinator<int>, T2>(Procrastinator<int> value);

}
}
#endif
