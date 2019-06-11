#ifndef PROCESS_H
#define PROCESS_H

#include <stack>
#include <memory>
#include <sst/core/event.h>
#include "Sim_Events.h"
//#include "Events/terminateEvent.h"


namespace SST{
namespace BEComponent{

class Process : public std::enable_shared_from_this<Process>
{
public:

	std::shared_ptr<Process> parent;	
	std::stack<std::shared_ptr<Process>> children;
        std::string type;
        std::map<std::string, double> state;

	virtual std::shared_ptr<simEvent> run(){
	    std::shared_ptr<simEvent> event;
            if(type == "Master Wait Process")
                event = std::make_shared<waitEvent>();
            else
                event = std::make_shared<terminateEvent>();
            return event;
	}
	
	Process(){
	    parent = nullptr;
            type = "Master Terminate Process";
	}
		
	/*std::shared_ptr<SST::Event> next(){
		return run(); 
	}*/
	
	void append(std::shared_ptr<Process> other){
	    children.push(other);
	    other->parent = shared_from_this();
	}
	
	void selfDelete(){
	    parent->children.pop(); //which will it pop?
	}

	
	~Process(){}		
};

}
}
#endif //PROCESS_H

