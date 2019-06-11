#include "Message.h"
//#include "Routine.h"
#include "Sim_Events.h"

using namespace SST;
using namespace SST::BEComponent;


std::shared_ptr<simEvent> Message::run(){

	while (!event_queue.empty()){
		std::shared_ptr<simEvent> event = event_queue.front();
		event_queue.pop();
		return event;
	}

	return NULL;
}
