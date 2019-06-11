#ifndef SIMULATION_MANAGER_H
#define	SIMULATION_MANAGER_H

#include <map>
#include <vector>
#include <queue>
#include <tuple>
#include <list>
#include <memory>
#include <boost/python.hpp>
#include <sst/core/event.h>
#include "Sim_Events.h"
#include "Process_Events.h"
#include "Event_Template.h"
#include "Process.h"
#include <Executor.h>
#include <Routine.h>
#include "Message.h"
#include "Layout.h"
#include "Router.h"
#include "Containers.h"

namespace SST {
namespace BEComponent {

class Executor;
class Message;

class ArgFlags
{

public:
    ArgFlags()
    {
	interpolation_scheme = "linear";
        output_file = "";
        traceOutput = "";
        debug = -1;
    }

    std::string interpolation_scheme;
    std::string output_file;
    std::string traceOutput;
    int debug;

};


class simManager
{

public:
    simManager(int gid, int cid, int ordinal, int par, std::string ops, std::string rels, std::string props, std::string mbxes, 
               std::string topo, std::string cd_list, std::string p_list, std::string m_list, std::string c_list, std::string s_flags, 
               PyObject* myFunc, std::function<void(std::map<std::tuple<std::string, std::vector<float>>, std::vector<double>>)> updCache, 
               std::function<std::map<std::tuple<std::string, std::vector<float>>, std::vector<double>>()> gtCache)
    {
                executor_id = 0;
                self_gid = gid;
                self_ordinal = ordinal;
                myFunction = myFunc;
                updateCache = updCache;
                getCache = gtCache;
                //currentSimTime = currentTime;

                sim_flags = new ArgFlags();
                
                //if(self_gid > 0) std::cout<<self_gid<<"- Component properties build started!\n";
                buildInformation(ops, rels, props, mbxes, cd_list, p_list, m_list, c_list, s_flags);
                //if(self_gid > 0) std::cout<<self_gid<<"- Component properties build successful!\n";	
		
                /* Build dynamic router for all components */
                if(self_ordinal == -2) dynamicRouter = new DynamicRouter(topo, cid, par, containerDimensions, plusNeighbour, minusNeighbour, children); // ordinal is -2 for containers

                else dynamicRouter = new DynamicRouter(topo, self_ordinal, par, containerDimensions, plusNeighbour, minusNeighbour, children);

                /* do not forget to build this from config */
		hardware_state["usage"] = 0.0;
		hardware_state["waiting"] = 0.0;  
		                	
    }
    
    ~simManager(){}

    DynamicRouter *dynamicRouter;

    ArgFlags *sim_flags;

    int self_gid, self_ordinal;

    PyObject* myFunction;

    std::vector<std::string> mailboxList;

    std::map<std::string, std::string> programs;

    std::tuple<std::string, std::tuple<bool, bool, bool>> mailboxes; //check if a single tuple is enough

    std::map<std::string, double> hardware_state;
    
    std::map<std::string, std::tuple<std::string, std::string, std::queue<std::shared_ptr<eventTemplate>>>> operations;

    std::map<std::string, int> relations;

    std::map<std::string, std::string> properties;

    std::vector<int> containerDimensions, plusNeighbour, minusNeighbour, children;

    std::map<std::tuple<std::string, std::vector<float>>, std::vector<double>> lookup_cache;
    // indicators
    
    
    int giveExecutorId() { return ++executor_id; }
    
    std::tuple<std::vector<double>, std::queue<std::shared_ptr<eventTemplate>>> lookup(std::string operation, std::vector<float> inputs);

    PyObject* vectorToList(std::vector<float> data);

    std::vector<double> listToVector(PyObject* incoming);

    bool vectorCheck(std::vector<float> list1, std::vector<float> list2);

    std::queue<std::shared_ptr<Process>> mailbox_routines (std::vector<int>, int, int, int, int, int, std::string);

    std::queue<std::shared_ptr<Process>> dynamic_mailbox_routines (std::vector<int>, std::vector<int>, int, int, int, int, int, std::string, bool);

    std::queue<std::shared_ptr<Process>> ack_routines (std::vector<int> gids, int pid, int source, int target, int size, int tag, std::string comm_type);

    int obtain(std::string property);

    float fobtain(std::string property);

    void buildInformation(std::string, std::string, std::string, std::string, std::string, std::string, std::string, std::string, std::string);

    std::shared_ptr<eventTemplate> buildTemplate(std::vector<std::string> t_list);

    std::tuple<std::string, Procrastinator*> value_find(std::vector<std::string> list);

    std::vector<std::string> decode(std::string operand, std::string delimiter);

    std::shared_ptr<Routine> call(int eventId, int source_gid, int source_pid, int target_gid, std::string target, std::string operation, std::vector<float> inputs, std::string call_type);

    std::shared_ptr<Message> comm(int gid, int pid, int eventId, std::string operation, int size, int target_rank, int tag, std::string comm_type);

    std::shared_ptr<Executor> prog(std::string filename);
    
    std::function<void(std::map<std::tuple<std::string, std::vector<float>>, std::vector<double>>)> updateCache;
  
    std::function<std::map<std::tuple<std::string, std::vector<float>>, std::vector<double>>()> getCache;

    std::function<int()> currentSimTime;

    bool stringToBool(std::string str) const {
        if(str == "True") return true;
        else return false;
    }

    std::vector<float> mailbox_function(int source, int target, int size, int tag) {
        std::vector<float> list;
        for(int i=0; i<mailboxList.size(); i++) {
            if(mailboxList[i] == "'source'") list.push_back(source);
            else if(mailboxList[i] == "'target'") list.push_back(target);
            else if(mailboxList[i] == "'size'") list.push_back(size);
            else if(mailboxList[i] == "'tag'") list.push_back(tag);
        }
        return list;
    }
 

private:
   
    int executor_id;

};  

                 
}
}

#endif	/* SIMULATOR_H */

