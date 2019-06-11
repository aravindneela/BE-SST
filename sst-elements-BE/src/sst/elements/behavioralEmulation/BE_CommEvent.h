#ifndef _BECOMMEVENT_H
#define _BECOMMEVENT_H

#include "Sim_Events.h"
#include "Process.h"

namespace SST {
namespace BEComponent {

class beCommEvent : public SST::Event 
{
public:

  //std::shared_ptr<simEvent> event;
  //std::shared_ptr<Process> eventProcess;
  double eventTime;
  int source, target, comp_id;
  std::string type, op_param, op_id, sub_type;
  std::vector<int> list, tarlist;

  beCommEvent() : SST::Event() {;}

public:
    void serialize_order(SST::Core::Serialization::serializer &ser) {
        Event::serialize_order(ser);
        ser & eventTime;
        ser & source;
        ser & target;
        ser & comp_id;
        ser & type;
        ser & op_param;
        ser & op_id;
        ser & sub_type;
        ser & list;
        ser & tarlist;
    }

    ImplementSerializable(SST::BEComponent::beCommEvent);
};

} // namespace beComponent
} // namespace SST

#endif /* _BECOMMEVENT_H */
