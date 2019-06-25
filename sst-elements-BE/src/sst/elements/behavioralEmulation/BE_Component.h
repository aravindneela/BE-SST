
#ifndef _BECOMPONENT_H
#define _BECOMPONENT_H

//#include <boost/python.hpp>
#include <assert.h>
#include <fstream>
#include <sst/core/component.h>
#include <sst/core/link.h>
#include "BE_CommEvent.h"
#include "Layout.h"
#include <Sim_Events.h>
#include <Process_Events.h>
#include <Executor.h>


namespace SST {
namespace BEComponent {

class beComponent : public SST::Component 
{
public:

    beComponent(SST::ComponentId_t id, SST::Params& params);

    ~beComponent();

    int getGid() { return self_gid; }

    int getOrdinal() { return self_ordinal; }

    std::string getKind() { return self_kind; }

    std::string getOperations() { return operations; }

    std::string getRelations() { return relations; }

    std::string getProperties() { return properties; }

    static int getCurrentTime();     

    void setup();

    virtual bool clockTic(SST::Cycle_t);

    void linkRecvEvent(Event *ev); 

    void build(Layout* layout, std::string file);

    std::vector<std::string> decode(std::string operand, std::string delimiter);

    void generateTrace(std::shared_ptr<simEvent> event, std::shared_ptr<Process> process, bool endOfEvent, bool messagePathTrace, std::tuple<int, int, int, std::vector<int>>);

    bool running(bool flg);

    void step(std::shared_ptr<Process> process);

    void tick(double eventTime, std::shared_ptr<simEvent> event, std::shared_ptr<Process> eventProcess);

    void run(std::shared_ptr<Process> process);

    void handleEvents(std::shared_ptr<simEvent> event, std::shared_ptr<Process> eventProcess);

    void finish(); 


private:

    beComponent();  // for serialization only
    beComponent(const beComponent&); // do not implement
    void operator=(const beComponent&); // do not implement
    
    beCommEvent* buildLinkEvent(std::string, int, int, std::vector<int>, std::vector<int>, int, std::string, std::string, std::string);
    
    int self_gid, self_cid, num_links, self_ordinal, self_clock, parent;

    std::string sys_layout, sys_flags, self_kind, program_file, link_list, operations, relations, properties, mailboxes, attributes, containerDimension_list, plus_list, minus_list, topology;
    std::string children_s;

    std::shared_ptr<Executor> executor;

    std::shared_ptr<simManager> simulation_handler;

    std::shared_ptr<Process> componentProcess;

    Layout* layout;

    double self_time, sim_time;

    int key, barrierCount;

    std::queue<std::tuple<double, std::shared_ptr<timeoutEvent>, std::shared_ptr<Process>>> computeQ;

    std::queue<std::tuple<std::shared_ptr<timeoutEvent>, std::shared_ptr<Process>>> computeWaitQ; 
   
    std::queue<std::tuple<std::shared_ptr<simEvent>, std::shared_ptr<Process>>> waitQ;

    std::queue<std::tuple<std::shared_ptr<simEvent>, std::shared_ptr<Process>>> barrierQ;

    std::queue<int> barrierList;

    std::map<int, std::tuple<std::shared_ptr<conditionEvent>, std::shared_ptr<Process>>> watchList;

    std::map<int, std::queue<std::tuple<std::shared_ptr<simEvent>, std::shared_ptr<Process>>>> messageTable;

    std::map<int, std::queue<std::tuple<std::shared_ptr<simEvent>, std::shared_ptr<Process>>>> recvTable;

    //std::vector< std::tuple< int, std::string, std::tuple<int, int, int>, std::tuple<int, int>, std::vector< std::tuple<int, std::tuple<double, double>, std::string, std::vector<int>, std::vector<int> > > > > comm_records;
   
    //std::vector< std::tuple< int, std::string, std::vector<int>, std::string, std::vector<double>, std::string, std::vector<int> > > call_records;

    std::map<int, SST::Link*> C_Link;

    SST::Link* selfEventLink;
    
};

} // namespace BEComponent
} // namespace SST

#endif /* _BECOMPONENT_H */
