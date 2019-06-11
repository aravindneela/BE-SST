#ifndef PROCESS_EVENTS_H
#define PROCESS_EVENTS_H

#include<sst/core/event.h>
#include <queue>
#include "Process.h"
#include "Sim_Events.h"

namespace SST {
namespace BEComponent {


class autoprocessEvent : public simEvent {	
public: 
    autoprocessEvent(std::queue<std::shared_ptr<Process>> p) { 
    	processes = p;
        type = "autoprocess";
    }
    
    std::queue<std::shared_ptr<Process>> processes;

};


class subprocessEvent : public simEvent {
public:
    subprocessEvent(std::queue<std::shared_ptr<Process>> p) { 
    	processes = p;
        type = "subprocess";
    }

    std::queue<std::shared_ptr<Process>> processes;

};


}
}

#endif
 


