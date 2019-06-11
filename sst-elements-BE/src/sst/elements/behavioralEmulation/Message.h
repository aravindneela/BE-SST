#ifndef MESSAGE_H
#define MESSAGE_H

#include "Process.h"
#include <vector>
#include <queue>
#include <map>
#include <tuple>
#include <memory>
#include "Routine.h"
#include "Simulation_Manager.h"
#include "Sim_Events.h"
#include "Process_Events.h"
#include "Event_Template.h"
#include <sst/core/event.h>

namespace SST {
namespace BEComponent {

class simManager;
class simEvent;

class Message : public Process{
public:
	explicit Message(int g, int p, int e, int s, int r, int sz, int tag, std::vector<int> slocations, std::string comm_type, std::map<std::string, double> h_state) : Process(){
		gid = g;
                pid = p;
                eventId = e;
		source = s;
		target = r;
		size = sz;
		tag = tag;
                locations = slocations;
                type = "message";
                state = h_state;
                //std::shared_ptr<simEvent> route_ev = std::make_shared<routeEvent>(gid, pid, source, target, locations); 
                //event_queue.push(route_ev);
                std::vector<int> empty;
                std::shared_ptr<simEvent> comm_ev = std::make_shared<commEvent>(eventId, source, size, target, tag, empty, locations, -1, comm_type); 
                event_queue.push(comm_ev);
                if(comm_type == "blocking"){
                    std::shared_ptr<simEvent> wait = std::make_shared<waitEvent>();
                    event_queue.push(wait);
                }
                
	}

	~Message(){}

	std::shared_ptr<simEvent> run();
        int gid, pid, eventId;
        std::vector<int> locations;
        
        std::tuple<int, int, int, int> getMessageParams() {
            return std::tie(source, target, size, tag);
        }
 
private:
	int source, target, size, tag;
        std::queue<std::shared_ptr<simEvent>> event_queue;

};

}
}
#endif
