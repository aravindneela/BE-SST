#include "sst_config.h"
#include "BE_Component.h"
#include "Process.h"
#include "Routine.h"
#include "Message.h"
#include "Simulation_Manager.h"
#include "Router.h"
#include <typeinfo>
#include <functional>
#include <ctime>
#include <csignal>
#include "sst/core/event.h"
#include <string>

using namespace SST;
using namespace SST::BEComponent;

beComponent::beComponent(ComponentId_t id, Params& params) :
  Component(id) 
{
   //printf("Inside the SST component constructor \n");
   bool found;

   self_gid = params.find<int64_t>("Component gid", 0, found);
    if (!found) {
        Simulation::getSimulation()->getSimulationOutput().fatal(CALL_INFO, -1,"couldn't find parameter gid\n");
    }

   self_cid = params.find<int64_t>("Component cid", 0, found);
    if (!found) {
        Simulation::getSimulation()->getSimulationOutput().fatal(CALL_INFO, -1,"couldn't find parameter cid\n");
    }

   if( self_gid == 0) std::cout<<"STARTED LAYOUT BUILD!!!\n";

   sys_layout = params.find<std::string>("System Layout", "none", found);
    if (!found) {
        Simulation::getSimulation()->getSimulationOutput().fatal(CALL_INFO, -1,"couldn't find parameter layout\n");
    }

   sys_flags = params.find<std::string>("Flags", "none", found);
    if (!found) {
        Simulation::getSimulation()->getSimulationOutput().fatal(CALL_INFO, -1,"couldn't find parameter flags\n");
    }

   self_clock = params.find<int64_t>("Component clock", 0, found);
    if (!found) {
        Simulation::getSimulation()->getSimulationOutput().fatal(CALL_INFO, -1,"couldn't find parameter clock\n");
    } 

   self_kind = params.find<std::string>("Component kind", "none", found);
    if (!found) {
        Simulation::getSimulation()->getSimulationOutput().fatal(CALL_INFO, -1,"couldn't find parameter kind\n");
    }

   self_ordinal = params.find<int64_t>("Component ordinal", 0, found);
    if (!found) {
        Simulation::getSimulation()->getSimulationOutput().fatal(CALL_INFO, -1,"couldn't find parameter ordinal\n");
    }

   attributes = params.find<std::string>("Attributes", "none", found);
    if (!found) {
        Simulation::getSimulation()->getSimulationOutput().fatal(CALL_INFO, -1,"couldn't find parameter Attributes\n");
    }

   operations = params.find<std::string>("Operations", "none", found);
    if (!found) {
        Simulation::getSimulation()->getSimulationOutput().fatal(CALL_INFO, -1,"couldn't find parameter operations\n");
    }

   relations = params.find<std::string>("Relations", "none", found);
    if (!found) {
        Simulation::getSimulation()->getSimulationOutput().fatal(CALL_INFO, -1,"couldn't find parameter relations\n");
    }

   properties = params.find<std::string>("Properties", "none", found);
    if (!found) {
        Simulation::getSimulation()->getSimulationOutput().fatal(CALL_INFO, -1,"couldn't find parameter properties\n");
    }

   mailboxes = params.find<std::string>("Mailboxes", "none", found);
    if (!found) {
        Simulation::getSimulation()->getSimulationOutput().fatal(CALL_INFO, -1,"couldn't find parameter mailboxes\n");
    }

   num_links = params.find<int64_t>("num_links", 0, found);
    if (!found) {
        Simulation::getSimulation()->getSimulationOutput().fatal(CALL_INFO, -1,"couldn't find parameter num_links\n");
    }

   topology = params.find<std::string>("Topology", "none", found);
    if (!found) {
        Simulation::getSimulation()->getSimulationOutput().fatal(CALL_INFO, -1,"couldn't find parameter topology\n");
    }

   link_list = params.find<std::string>("Link list", "none", found);
    if (!found) {
        Simulation::getSimulation()->getSimulationOutput().fatal(CALL_INFO, -1,"couldn't find parameter link list\n");
    }

   plus_list = params.find<std::string>("Plus links", "none", found);
    if (!found) {
        Simulation::getSimulation()->getSimulationOutput().fatal(CALL_INFO, -1,"couldn't find parameter plus links\n");
    }

   minus_list = params.find<std::string>("Minus links", "none", found);
    if (!found) {
        Simulation::getSimulation()->getSimulationOutput().fatal(CALL_INFO, -1,"couldn't find parameter minus links\n");
    }

    parent = params.find<int64_t>("Parent", 0, found);
    if (!found) {
        Simulation::getSimulation()->getSimulationOutput().fatal(CALL_INFO, -1,"couldn't find parameter parent\n");
    }

    children_s = params.find<std::string>("Children", "none", found);
    if (!found) {
        Simulation::getSimulation()->getSimulationOutput().fatal(CALL_INFO, -1,"couldn't find parameter Children\n");
    }

   containerDimension_list = params.find<std::string>("Container dimensions", "none", found);
    if (!found) {
        Simulation::getSimulation()->getSimulationOutput().fatal(CALL_INFO, -1,"couldn't find parameter container dimensions\n");
    }

   program_file = params.find<std::string>("Software Program", "none", found);
    if (!found) {
        Simulation::getSimulation()->getSimulationOutput().fatal(CALL_INFO, -1,"couldn't find parameter Program\n");
    }

    self_time = 0.0;
    key = 0;
    
    componentProcess = std::make_shared<Process>();
   
    /* Configure the self link for computation events in each Component */
    selfEventLink = configureSelfLink("Selflink", new Event::Handler<beComponent>(this, &beComponent::linkRecvEvent));

    /* register the component clock */
    registerClock(std::to_string(self_clock)+"Hz", new Clock::Handler<beComponent>(this, 
                  &beComponent::clockTic));

    if( self_gid == 0) {
        barrierCount = 0;
        std::cout<<"LAYOUT BUILD OVER!!!\n";
    }

}


beComponent::~beComponent() 
{

}


beComponent::beComponent() : Component(-1)
{
    // for serialization only
}


void beComponent::setup()
{

    /* Python lookup module setup */
    Py_Initialize();
    PyRun_SimpleString("import sys");
    PyRun_SimpleString("sys.path.append(\"/home/ajay/scratch/src/sst-elements-library-6.0.0/src/sst/elements/behavioralEmulation/tests\")");
    PyObject* myModuleString = PyString_FromString((char*)"lookup");
    PyObject* myModule = PyImport_Import(myModuleString);
    PyObject* myFunction = PyObject_GetAttrString(myModule,(char*)"lookupValue");
    
    /* Lookup cache setup */
    std::function<void(std::map<std::tuple<std::string, std::vector<float>>, std::vector<double>>)> updateCache = updateGlobalLookupCache;
    std::function<std::map<std::tuple<std::string, std::vector<float>>, std::vector<double>>()> getCache = getGlobalLookupCache;

    /* Build the simulation handler for the component */
    //if(self_gid > 0) std::cout<<"\n"<<self_gid<<"- Simulation handler build started!\n";
    simulation_handler = std::make_shared<simManager>(self_gid, self_cid, self_ordinal, parent, operations, relations, properties, 
                                                      mailboxes, topology, containerDimension_list, plus_list, minus_list, 
                                                      children_s, sys_flags, myFunction, updateCache, getCache);

    if((simulation_handler->sim_flags)->debug == self_gid) std::cout<<"GID: "<<self_gid<<" -- Simulation handler build successful!\n";

    /* configure out links */
    std::vector<std::string> link_gids;
    int l_gid;
    link_gids = decode(link_list.substr(1, link_list.size()-2), ", ");

    for(int i = 0; i < num_links; i++){
        l_gid = stoi(link_gids[i]);
        C_Link[l_gid] = configureLink("Link "+link_gids[i], new Event::Handler<beComponent>(this, &beComponent::linkRecvEvent));
        assert(C_Link[l_gid]);
    }

    /* Set up executor process if there is an application program to be run */
    if(program_file != "none")
    {
      // tell the simulator not to end without us if component has an executor process
      primaryComponentDoNotEndSim();

      std::shared_ptr<Executor> executor = std::make_shared<Executor>(self_gid, simulation_handler->giveExecutorId(), program_file, simulation_handler, simulation_handler->hardware_state);

      if((simulation_handler->sim_flags)->debug == self_gid) std::cout<<"GID: "<<self_gid<<" -- Simulation handler build successful!\n";

      componentProcess->append(executor);

    }

    else
    {
      componentProcess->type = "Master Wait Process";
    }

    /* Run the simulation */
    run(0, 0, 0);    

}


std::vector<std::string> beComponent::decode(std::string operand, std::string delimiter)
{	

    size_t pos = 0;
    std::vector<std::string> operandList;

    while((pos = operand.find(delimiter)) != std::string::npos){           
        operandList.push_back(operand.substr(0, pos));
        operand.erase(0, pos + delimiter.length());
    }
    operandList.push_back(operand);

    return operandList;
            
}


void beComponent::build(Layout* layout, std::string file)
{
    
    std::vector<std::string> list1; 
    

    list1 = decode(file, "\n");
    
    for(int i=0; i < 12; i++){

        ////printf("loop iteration %d\n", i);
        std::vector<std::string> list2, list3, list4, list5;

        if(i==2 || i==4){

            switch(i){
     
                case 2: list2 = decode(list1[i].substr(3, list1[i].size()-6), "]], [[");
                        for(auto itr1 = list2.begin(); itr1 != list2.end(); itr1++)
                        {
                            std::string temp1 = *itr1;
                            std::vector<std::vector<int>> sublist1;
                            list3 = decode(temp1, "], [");
                            for(auto itr2 = list3.begin(); itr2 != list3.end(); itr2++){
                                std::string temp2 = *itr2;
                                std::vector<int> sublist2;
                                list4 = decode(temp2, ", ");
                                for(auto itr3 = list4.begin(); itr3 != list4.end(); itr3++)
                                    if(*itr3 != "")
                                      sublist2.push_back(stoi(*itr3));                                
                                
                                sublist1.push_back(sublist2);
                            }                                
                            layout->edges.push_back(sublist1);
                               
                        }
                        break;

                case 4: list2 = decode(list1[i].substr(2, list1[i].size()-4), "], [");
                        for(auto itr1 = list2.begin(); itr1 != list2.end(); itr1++)
                        {
                            std::string temp1 = *itr1;
                            std::vector<int> sublist1;
                            list3 = decode(temp1, ", ");
                            for(auto itr2 = list3.begin(); itr2 != list3.end(); itr2++)
                                if(*itr2 != "")
                                  sublist1.push_back(stoi(*itr2));                                
                            layout->children.push_back(sublist1);
                               
                        }
                        break;
           
            }


        } 

        else if(i==0 || i==1 || i==3){

            if(i==3)
                list2 = decode(list1[i].substr(1, list1[i].size()-2), ", "); 

            switch(i){

                /*case 0: for(auto itr1 = list2.begin(); itr1 != list2.end(); itr1++){
                            if(*itr1 != "")
                              layout->cid.push_back(stoi(*itr1));
                        }
                        break;*/

                /*case 1: for(auto itr1 = list2.begin(); itr1 != list2.end(); itr1++){
                            std::string temp1 = *itr1;
                            if(*itr1 != "")
                              layout->kinds.push_back(temp1.substr(1, temp1.size()-2));
                        }
                        break;*/

                case 3: for(auto itr1 = list2.begin(); itr1 != list2.end(); itr1++){
                            if(*itr1 == "null")
                              layout->parents.push_back(-1);
                            else if(*itr1 != "")
                              layout->parents.push_back(stoi(*itr1));
                        }
                        break;

                default: break;
 
            }
      
        }    

        else if(i==6 || i==9 || i==10){

            list2 = decode(list1[i].substr(1, list1[i].size()-2), ", ");
            
            switch(i){

                case 6: for(auto itr1 = list2.begin(); itr1 != list2.end(); itr1++)
                        {
                            std::string temp1 = *itr1;
                            std::string key = "", value = "";
                            list3 = decode(temp1, ": ");
                            if(list3[0].size() > 2)
                                key = list3[0].substr(1, list3[0].size()-2);
                            if(list3.size() > 1)
                                value = list3[1].substr(1, list3[1].size()-2);
                            if(key != "")
                              layout->netnames[stoi(key)] = value;       
                        }
                        break;

                case 9: for(auto itr1 = list2.begin(); itr1 != list2.end(); itr1++)
                        {
                            std::string temp1 = *itr1;
                            std::string key = "", value = "";
                            list3 = decode(temp1, ": ");
                            if(list3[0].size() > 2)
                                key = list3[0].substr(1, list3[0].size()-2);
                            if(list3.size() > 1)
                                value = list3[1];
                            if(key != "" && value != "")
                              layout->ordinals[stoi(key)] = stoi(value);     
                        }
                        break;

                case 10: for(auto itr1 = list2.begin(); itr1 != list2.end(); itr1++)
                        {
                            std::string temp1 = *itr1;
                            std::string key = "", value = "";
                            list3 = decode(temp1, ": ");
                            if(list3[0].size() > 2)
                                key = list3[0].substr(1, list3[0].size()-2);
                            if(list3.size() > 1)
                                value = list3[1];
                            if(key != "" && value != "")
                              layout->rordinals[stoi(key)] = stoi(value);       
                        }
                        break;
 
            }           

        } 

        else if(i==8){
/*
            list2 = decode(list1[i].substr(1, list1[i].size()-3), "}, ");

            for(auto itr1 = list2.begin(); itr1 != list2.end(); itr1++){

                std::string temp1 = *itr1;
                list3 = decode(temp1, ": {");
                if(list3.size() > 1)
                    list4 = decode(list3[1], ", ");
                std::map<std::string, int> submap;

                for(auto itr2 = list4.begin(); itr2 != list4.end(); itr2++){
                    std::string temp2 = *itr2;
                    std::string key = "", value = "";
                    list5 = decode(temp2, ": ");
                    if(list5[0].size() > 2)
                        key = list5[0].substr(1, list5[0].size()-2);
                    if(list5.size() > 1)
                        value = list5[1];
                    if(value != "")
                      submap[key] = stoi(value); 
                }

                std::string pkey = "";
                if(list3[0].size() > 2)
                    pkey = list3[0].substr(1, list3[0].size()-2);
                if(pkey != "")
                  layout->relations[stoi(pkey)] = submap;

            }
*/;
        }

        else{

            list2 = decode(list1[i].substr(1, list1[i].size()-3), "], ");

            for(auto itr1 = list2.begin(); itr1 != list2.end(); itr1++){

                std::string temp1 = *itr1;
                std::string pkey = "";

                list3 = decode(temp1, ": [");

                if(list3[0].size() > 2)
                    pkey = list3[0].substr(1, list3[0].size()-2);
                if(list3.size() > 1)
                    list4 = decode(list3[1], ", ");

                std::vector<int> sublist;

                for(auto itr2 = list4.begin(); itr2 != list4.end(); itr2++){
                    if(*itr2 != "")
                      sublist.push_back(stoi(*itr2)); 
                }


                switch(i){

                  case 5: if(pkey != "")
                            layout->indices[stoi(pkey)] = sublist;
                          break;

                  case 7: if(pkey != "")
                            layout->netsizes[stoi(pkey)] = sublist;
                          break;

                  case 11: if(pkey != "")
                           layout->subordinals[stoi(pkey)] = sublist;
                           break;
 
                }

            }           

        }

    }
 
}


/*void beComponent::generateTrace(std::shared_ptr<simEvent> event, std::shared_ptr<Process> process, bool endOfEvent, bool messagePathTrace, std::tuple<int, int, int, std::vector<int>> messageParams)
{

    if(endOfEvent)
    {
	
	if(messagePathTrace)	//Check for non blocking communication
        {
	    double endtime = getCurrentSimTimeNano()/1000000000.0;
            std::shared_ptr<Routine> routine = std::dynamic_pointer_cast<Routine>(process);

	    for(int i = comm_records.size()-1; i >= 0; i--) 
            {
		if( (std::get<2>(std::get<2>(comm_records[i])) == routine->eventId) && (std::get<1>(std::get<2>(comm_records[i])) == routine->gid) ) {
                    std::get<1>(std::get<1>((std::get<4>(comm_records[i]))[0])) = endtime; //comm_records[i][-1][-1][1][-1] = endtime
                    break;
                }
            }

        }

        else
        {
	    double endtime = getCurrentSimTimeNano()/1000000000.0;
            std::shared_ptr<Routine> routine = std::dynamic_pointer_cast<Routine>(process);
            
            for(int i = call_records.size()-1; i >= 0; i--) 
            {
		if((std::get<2>(call_records[i]))[1] == routine->eventId) {
                    (std::get<4>(call_records[i])).back() = endtime; //call_records[i][4][-1] = endtime
                    break;
                }
            }

        }

    }

    else
    {
	
	if(messagePathTrace)
        {
            std::vector< std::tuple<int, std::tuple<double, double>, std::string, std::vector<int>, std::vector<int>> > message_path;

            if(event == NULL)
            {
                double starttime = getCurrentSimTimeNano()/1000000000.0;

                std::tuple<double, double> time_pair = std::make_tuple(starttime, starttime);
                std::string lookup_file = "None";
                std::vector<int> parameters;
                std::vector<int> locations = std::get<3>(messageParams);

                std::tuple<int, int, int> sid_pair = std::make_tuple(std::get<0>(messageParams), locations[0], std::get<2>(messageParams));
                std::tuple<int, int> rid_pair = std::make_tuple(std::get<1>(messageParams), 0); 

		message_path.push_back(make_tuple(self_gid, time_pair, lookup_file, parameters, locations));

                comm_records.push_back(make_tuple(locations[0], "comm", sid_pair, rid_pair, message_path));

            }

            else
            {
                std::shared_ptr<subprocessEvent> ev = std::dynamic_pointer_cast<subprocessEvent>(event);
                std::shared_ptr<Routine> routine = std::dynamic_pointer_cast<Routine>((ev->processes).front());     // will we ever have more than one routine in interconnect component

                std::vector<int> parameters = std::get<0>(routine->getRoutineParams());
                parameters.push_back(std::get<2>(messageParams));

                std::string lookup_file = std::get<0>(simulation_handler->operations[std::get<0>(simulation_handler->mailboxes)]);
           
                double starttime = getCurrentSimTimeNano()/1000000000.0;

                std::tuple<double, double> time_pair = std::make_tuple(starttime, 0.0);
                std::tuple<int, int, int> sid_pair = std::make_tuple(std::get<0>(messageParams), routine->gid, routine->eventId);
                std::tuple<int, int> rid_pair = std::make_tuple(std::get<1>(messageParams), 0); 
                std::vector<int> locations = std::get<3>(messageParams);

                message_path.push_back(make_tuple(self_gid, time_pair, lookup_file, parameters, locations));

                comm_records.push_back(make_tuple(routine->gid, "comm", sid_pair, rid_pair, message_path));
            }

        }

        else
        {
            std::shared_ptr<Executor> executor = std::dynamic_pointer_cast<Executor>(process);
	    std::vector<Instruction>::iterator instruction = executor->getCurrentInstruction();
            std::map<std::string, int> registers = executor->getRegisterSet();
            double starttime;

            if(instruction->kind == "call")
            {
		std::string event_type = (instruction->operands)[1];
                std::vector<int> parameters;
                std::shared_ptr<subprocessEvent> ev = std::dynamic_pointer_cast<subprocessEvent>(event);
                std::shared_ptr<Routine> routine = std::dynamic_pointer_cast<Routine>((ev->processes).front());

                for(int i = 2; i < (instruction->operands).size(); i++) {
                    if(((instruction->operands)[i])[0] == 'r') parameters.push_back(registers[(instruction->operands)[i]]);
                    else parameters.push_back(stoi((instruction->operands)[i]));
                }

                int event_id = routine->eventId;
                int thread_id = self_cid;
                starttime = getCurrentSimTimeNano()/1000000000.0;
                std::string lookup_file = std::get<0>(simulation_handler->operations[event_type]);

                std::vector<int> id_pair;
                std::vector<double> time_pair;
                id_pair.push_back(thread_id);
                id_pair.push_back(event_id);
                time_pair.push_back(starttime);
                time_pair.push_back(0.0);

                call_records.push_back(make_tuple(self_gid, instruction->kind, id_pair, event_type, time_pair, lookup_file, parameters));
            }

            else if(instruction->kind == "comm")
            {
                std::shared_ptr<subprocessEvent> ev = std::dynamic_pointer_cast<subprocessEvent>(event);
                std::shared_ptr<Message> message = std::dynamic_pointer_cast<Message>((ev->processes).front());
                int sthread_id = self_cid;
                int sevent_id = message->eventId;
                int rthread_id;
                starttime = getCurrentSimTimeNano()/1000000000.0;

                if(((instruction->operands)[2])[0] == 'r') rthread_id = registers[(instruction->operands)[2]];
                else rthread_id = stoi((instruction->operands)[2]);

                std::vector< std::tuple<int, std::tuple<double, double>, std::string, std::vector<int>, std::vector<int>> > message_path;
                std::vector<int> parameters;

                std::tuple<double, double> time_pair = std::make_tuple(starttime, starttime);
                std::tuple<int, int, int> sid_pair = std::make_tuple(sthread_id, self_gid, sevent_id);
                std::tuple<int, int> rid_pair = std::make_tuple(rthread_id, 0);
                std::vector<int> locations;

                message_path.push_back(make_tuple(self_gid, time_pair, "None", parameters, locations));

                comm_records.push_back(make_tuple(self_gid, instruction->kind, sid_pair, rid_pair, message_path));
            }          

        }

    }
    
} */


beCommEvent* beComponent::buildLinkEvent(std::string type, int so, int tar, std::vector<int> tlist, std::vector<int> list, int coid, std::string opp, std::string opid, std::string st){

    beCommEvent* bev = new beCommEvent();

    bev->type 		= type;
    bev->source 	= so;
    bev->target 	= tar;
    bev->tarlist	= tlist;
    bev->list 		= list;
    bev->comp_id 	= coid;
    bev->op_param 	= opp;
    bev->op_id 		= opid;
    bev->sub_type 	= st;
    
    return bev;
    
}


void beComponent::step(std::shared_ptr<Process> process){

    //std::raise(SIGINT);
    if((simulation_handler->sim_flags)->debug == self_gid) std::cout<<"GID: "<<self_gid<<" -- Stepping through " << process->type << std::endl; 
    std::shared_ptr<simEvent> event = process->run();

    if (event != nullptr)
    {

        if(process->type == "Executor") {
            std::vector<int> empty;
            //generateTrace(event, process, false, false, std::make_tuple(0, 0, 0, empty)); //generate event trace for compute events and communicate events at the source
        }

        std::string event_type = event->type;
	double delay;

        if((simulation_handler->sim_flags)->debug == self_gid) std::cout<<"GID: "<<self_gid<<" -- " << event_type << " event returned" << std::endl;
	
	if (event_type == "timeout")
        {
            auto ev = std::dynamic_pointer_cast<timeoutEvent>(event);	
            //delay = ev->value*self_clock;
            delay = ev->value;
	}

	else
            delay = 0;
			
	double eventTime = self_time + delay;		
		
	if (event_type == "condition")
        {
	    auto ev = std::dynamic_pointer_cast<conditionEvent>(event);
            double value = simulation_handler->hardware_state[ev->attribute];//ev->state[ev->attribute];

            if (ev->conditionFunction(value, ev->value))
            {
                if((simulation_handler->sim_flags)->debug == self_gid) std::cout<<"GID: "<<self_gid<<" -- Condition satisfied" << std::endl;
                tick(eventTime, ev, process);
            }
            else
            {
                if((simulation_handler->sim_flags)->debug == self_gid) std::cout<<"GID: "<<self_gid<<" -- Condition not satisfied---notifying the necessary components" << std::endl;
	        watchList[key] = make_tuple(ev, process);//do we need unique id for object?
                key++;
            }

        }

        else if (event_type == "wait")
        {
            auto ev = std::dynamic_pointer_cast<waitEvent>(event);
            waitQ.push(std::make_tuple(ev, process));//?????waiting[eventTime]
        }

        else if (event_type == "barrier")
        {         
            std::vector<int> empty;
            auto barrier_ev = std::dynamic_pointer_cast<barrierEvent>(event);

            beCommEvent *bev;
            bev = buildLinkEvent("barrier", barrier_ev->source, barrier_ev->commgroup, empty, empty, -1, "", "", "");

            barrierQ.push(std::make_tuple(barrier_ev, process));

            if((simulation_handler->sim_flags)->debug == self_gid) std::cout<<"GID: "<<self_gid<<" -- waiting on barrier primitive"<<"\n";

            C_Link[0]->send(bev);
        }

/*        else if (event_type == "route")
        {
            auto route_ev = std::dynamic_pointer_cast<routeEvent>(event);
            beCommEvent *bev;
            bev = buildLinkEvent(self_time, "route", route_ev->source, route_ev->target, route_ev->locations, route_ev->gid, "", std::to_string(route_ev->pid), "");
            routeQ.push(std::make_tuple(route_ev, process));

            if((simulation_handler->sim_flags)->debug == self_gid) std::cout<<"GID: "<<self_gid<<" -- source: "<<route_ev->source<<" and target: "<<route_ev->target<<" and gid: "<< route_ev->gid<<"\n";

            C_Link[0]->send(bev);
        }	
*/
        else if (event_type == "receiveWait")
        {
            auto ev = std::dynamic_pointer_cast<recvWaitEvent>(event);
            bool present = false;
            std::shared_ptr<Routine> routine = std::dynamic_pointer_cast<Routine>(process);

            int msg_source = std::get<0>(routine->getRoutineParams())[1];

            for(auto itr = messageTable.begin(); itr != messageTable.end(); itr++) {

                if (msg_source == itr->first && !((itr->second).empty())) {
                    std::shared_ptr<Process> mprocess;
                    std::shared_ptr<simEvent> mevent;
                    std::tie(mevent, mprocess) = (itr->second).front();
                    (itr->second).pop();
                    tick(self_time, mevent, mprocess);
                    tick(self_time, ev, process);
                    present = true;
                }

            }

            if(!present) (recvTable[msg_source]).push(std::tie(ev, process));
             
        }

        else if (event_type == "receive")
        {
            auto ev = std::dynamic_pointer_cast<recvEvent>(event);
            bool present = false;
            std::shared_ptr<Routine> routine = std::dynamic_pointer_cast<Routine>(process);

            if((std::get<0>(routine->getRoutineParams())).empty()) 
                throw std::runtime_error("Receive/unwait expecting the message source as input!! (Add source to the lambda function of unwait operation)");

            int msg_source = std::get<0>(routine->getRoutineParams())[0];

            for(auto itr = recvTable.begin(); itr != recvTable.end(); itr++) {

                if (msg_source == itr->first && !((itr->second).empty())) {
                    std::shared_ptr<Process> rprocess;
                    std::shared_ptr<simEvent> revent;
                    std::tie(revent, rprocess) = (itr->second).front();
                    (itr->second).pop();
                    tick(self_time, revent, rprocess);
                    tick(self_time, ev, process);
                    present = true;
                }

            }

            if(!present) (messageTable[msg_source]).push(std::tie(ev, process));
             
        }			

	else if (event_type != "timeout")
        {		
            tick(eventTime, event, process);
	}

	else
        {
            auto ev = std::dynamic_pointer_cast<timeoutEvent>(event);
            //upcoming.push(std::make_tuple(eventTime, ev, process));
            computeQ.push(std::make_tuple(eventTime, ev, process));
            //upcoming.pop();
            std::vector<int> empty;
            beCommEvent *bev;
            bev = buildLinkEvent(ev->type, -1, -1, empty, empty, -1, "", "", "");
            selfEventLink->send(ev->value*self_clock, bev);
	}

    }

    else
    {
        if((simulation_handler->sim_flags)->debug == self_gid) std::cout<<"GID: "<<self_gid<<" -- Event in this process is now null \n";

        if(process->type == "Routine") {
            std::shared_ptr<Routine> routine = std::dynamic_pointer_cast<Routine>(process);
            std::vector<int> empty;

            //if(process->parent->type == "Executor") generateTrace(NULL, process, true, false, std::make_tuple(0, 0, 0, empty));
            //else if(routine->creator == "comm") generateTrace(NULL, process, true, true, std::make_tuple(0, 0, 0, empty));
        }

        process->selfDelete();
  
	if (process->parent->children.empty())
	    step(process->parent);
	
	process->parent = nullptr;

    }

}

void beComponent::tick(double eventTime, std::shared_ptr<simEvent> event, std::shared_ptr<Process> eventProcess)
{

    if((simulation_handler->sim_flags)->debug == self_gid) std::cout<<"GID: "<<self_gid<<" -- Inside tick for the event " << event->type << std::endl;

    eventProcess->state = simulation_handler->hardware_state;
    //self_time = eventTime;

    handleEvents(event, eventProcess); 
    //std::tuple<double, std::shared_ptr<simEvent>> previous_set = std::make_tuple(eventTime, event);
    //eventList.push_back(previous_set);
	
    if (eventProcess->children.empty() && eventProcess->parent != NULL){
	step(eventProcess);
    }

}

void beComponent::walk(std::shared_ptr<Process> process){ 

        std::stack<std::shared_ptr<Process>> children_q = process->children; 

	if (!children_q.empty()){
		while(!children_q.empty()){ // for loop might be necessary
			walk(children_q.top());
                        children_q.pop();
		}
	}
	else
		step(process);
}


void beComponent::run(double t_cap=0.0, time_t w_cap=0.0, int e_cap=0)
{	

	walk(componentProcess);
	 
}


int beComponent::getCurrentTime()
{ 
    return 0;//(int) getCurrentSimTimeNano(); 
}


/* Does nothing during a clock tick as it is discrete event simulation */
bool beComponent::clockTic( Cycle_t )
{
    return false;                              
}


// incoming messages are scanned and deleted
void beComponent::linkRecvEvent(Event *ev) 
{

    beCommEvent *link_event = dynamic_cast<beCommEvent*>(ev);

    if((simulation_handler->sim_flags)->debug == self_gid) std::cout<<"GID: "<<self_gid<<" -- Event received here for "<<self_kind<<". It is "<<link_event->type<<"\n";

    if (link_event)
    {       

        if(link_event->type == "timeout")
        {
            std::tuple<double, std::shared_ptr<timeoutEvent>, std::shared_ptr<Process>> entry = computeQ.front(); 

            //double eventTime = std::get<0>(entry);

	    std::shared_ptr<simEvent> event = std::get<1>(entry);
	    std::shared_ptr<Process> eventProcess = std::get<2>(entry);

            computeQ.pop();

            tick(self_time, event, eventProcess);

            delete link_event;

        }

        else if(link_event->type == "communicate")
        {
            
            std::queue<std::shared_ptr<Process>> routines;
            bool isDestination = (self_ordinal == link_event->target);

            (link_event->list).push_back(self_gid);

            if((simulation_handler->sim_flags)->debug == self_gid) std::cout<<"GID: "<<self_gid<<" -- Event revd from "<<(link_event->list)[0]<<" (GID)"<<"\n";

            routines = simulation_handler->dynamic_mailbox_routines(link_event->list, link_event->tarlist, link_event->comp_id, link_event->source, link_event->target, stoi(link_event->op_param), stoi(link_event->op_id), link_event->sub_type, isDestination);

            if (!routines.empty())
            {
                std::shared_ptr<simEvent> routine_event = std::make_shared<subprocessEvent>(routines);
                tick(link_event->eventTime, routine_event, componentProcess);
            }

            else
            {
                int next_hop = -1;
                int hier_target = (link_event->tarlist).back();
                int targetOneLevelDown = -1;

                if((link_event->tarlist).size() > 1) targetOneLevelDown = (link_event->tarlist).end()[-2];

                /* If it is destination core - destination component creating acknowledge event if it is a blocking communication */
                if (isDestination)
                {
                    if(link_event->sub_type == "blocking") {
                        next_hop = link_event->list[(link_event->list).size() - 2];  // last element is the destination. So, send event to the one before that

                        beCommEvent *bev;
                        bev = buildLinkEvent("acknowledge", -1, -1, link_event->tarlist, link_event->list, 
                                             link_event->comp_id, "", "", "");

                        if(next_hop == self_gid) selfEventLink->send(bev);  
                        else if(next_hop >= 0) C_Link[next_hop]->send(bev);
                    }

                }

                else if(self_ordinal == -1) //not an unique classifier for links - change by passing kind of the BEO as a parameter to simulation!!
                {
                    int other_end = link_event->list[(link_event->list).size() - 2];
                 
                    for(auto itr = C_Link.begin(); itr != C_Link.end(); itr++) {
                        if(itr->first == other_end) continue;
                        else next_hop = itr->first;
                    }

                    beCommEvent *bev;
                    bev = buildLinkEvent(link_event->type, link_event->source, link_event->target, link_event->tarlist, link_event->list, 
                                         link_event->comp_id, link_event->op_param, link_event->op_id, link_event->sub_type);

                    if(next_hop >= 0) C_Link[next_hop]->send(bev);
                }
                
                /* If it is intermediate core or a container object*/
                else
                {
                    std::tuple<bool, int, int> route_set = (simulation_handler->dynamicRouter)->findNextHop(hier_target, targetOneLevelDown);
                    next_hop = std::get<1>(route_set);

                    if(std::get<0>(route_set) == 1) (link_event->tarlist).push_back(std::get<2>(route_set));

                    else if(std::get<0>(route_set) == -1) (link_event->tarlist).pop_back();

                    beCommEvent *bev;
                    bev = buildLinkEvent(link_event->type, link_event->source, link_event->target, link_event->tarlist, link_event->list, 
                                            link_event->comp_id, link_event->op_param, link_event->op_id, link_event->sub_type);

                    if(next_hop >= 0) C_Link[next_hop]->send(bev);
                }
            }   

            delete link_event;                   
          
        }

        else if(link_event->type == "acknowledge")  // acknowledge after the destination completes its routine?
        {
            //auto ack_ev = std::dynamic_pointer_cast<ackEvent>(link_event->event);
            std::vector<int> locations = link_event->list;
            std::queue<std::shared_ptr<Process>> routines;

            for(int j = 0; j < locations.size()-1; j++)
            {
                if( self_gid == locations[j])
                {
                    //routines = simulation_handler->ack_routines(locations, link_event->comp_id, link_event->source, link_event->target, 1, -1, link_event->sub_type);

		    if (!routines.empty())
                    {
                        std::shared_ptr<simEvent> routine_event = std::make_shared<subprocessEvent>(routines);

                        tick(link_event->eventTime, routine_event, componentProcess);
                    }

                    else
                    {
                        int next_hop = locations[j+1];

                        beCommEvent *bev;

                        bev = buildLinkEvent(link_event->type, -1, -1, link_event->tarlist, link_event->list, 
                                             link_event->comp_id, "", "", "");

                        //bev->event = ack_ev;
                        //bev->eventTime = self_time;

                        if(next_hop >= 0) C_Link[next_hop]->send(bev);
                    }

                    break;
                }
            }

            if (self_gid == locations.back() && !waitQ.empty())
            {
                //printf("Blocking Completion initiated by %d is complete\n", self_gid);
	        std::tuple<std::shared_ptr<simEvent>, std::shared_ptr<Process>> entry = waitQ.front();      //Change waitQ and acknowledge event to support process id
	        std::shared_ptr<simEvent> event;
	        std::shared_ptr<Process> eventProcess;

	        event = std::get<0>(entry);
	        eventProcess = std::get<1>(entry);

                waitQ.pop();
                if((simulation_handler->sim_flags)->debug == self_gid) std::cout<<"GID: "<<self_gid<<" -- received ack from "<<locations.back()<<" for "<<eventProcess->type<<"\n";

                tick(self_time, event, eventProcess); 
            }

            delete link_event;                

        }   

        else if(link_event->type == "barrier")
        {

            if( self_gid == 0)
            {
                int source = link_event->source;
                int group_count = link_event->target;

                barrierCount++;

                if(barrierCount < group_count)
                {
                    barrierList.push(source);
                }

                else 
                {
                    barrierList.push(source);
                    barrierCount = 0;
                    while(!barrierList.empty()) { 
                        beCommEvent *bev;
                        bev = buildLinkEvent(link_event->type, link_event->source, link_event->target, link_event->tarlist, link_event->list, 
                                             link_event->comp_id, link_event->op_param, link_event->op_id, link_event->sub_type);

                        C_Link[barrierList.front()]->send(bev);
                        barrierList.pop();
                    }        
                }
                
            }

            else
            {
                std::shared_ptr<Process> process; 
                std::tuple<std::shared_ptr<simEvent>, std::shared_ptr<Process>> entry = barrierQ.front();
	        std::shared_ptr<simEvent> event;

	        event = std::get<0>(entry);
	        process = std::get<1>(entry);

                barrierQ.pop(); 

                tick(self_time, event, process);
            }
                              
            delete link_event;

        } 

/*        else if(link_event->type == "route")
        {

            //auto route_ev = std::dynamic_pointer_cast<routeEvent>(link_event->event);
            if((simulation_handler->sim_flags)->debug == self_gid) std::cout<<"GID: "<<self_gid<<" -- this component received route from "<<link_event->comp_id<<"\n";

            if( self_gid == 0){
                std::vector<int> locations = simulation_handler->router->path(link_event->source, link_event->target);
                simulation_handler->router->clear();
       
                beCommEvent *bev;
                bev = buildLinkEvent(self_time, link_event->type, link_event->source, link_event->target, locations, 
                                     link_event->comp_id, link_event->op_param, link_event->op_id, link_event->sub_type);

                C_Link[link_event->comp_id]->send(bev);
            }

            else if( self_gid == link_event->comp_id){
                std::shared_ptr<Message> messageProcess; 
                std::tuple<std::shared_ptr<simEvent>, std::shared_ptr<Process>> entry = routeQ.front();
	        std::shared_ptr<simEvent> event;
	        event = std::get<0>(entry);
	        messageProcess = std::dynamic_pointer_cast<Message>(std::get<1>(entry));
                routeQ.pop(); 
                messageProcess->locations = link_event->list;
                if((simulation_handler->sim_flags)->debug == self_gid) std::cout<<"GID: "<<self_gid<<" -- Locations are: ";
                if((simulation_handler->sim_flags)->debug == self_gid) for(int i=0; i<(link_event->list).size(); i++) std::cout<<(link_event->list)[i]<<" ";
                tick(self_time, event, messageProcess);
            }
                              
            delete link_event;

        }      
*/
        else if(link_event->type == "call")
        {
            if((simulation_handler->sim_flags)->debug == self_gid) std::cout<<"GID: "<<self_gid<<" -- Call Event received here for "<<self_kind<<". Called by "<<link_event->source<<" for "<<link_event->op_id<<"\n";
 
            std::vector<int> integerInputs = link_event->list;
            std::vector<float> floatInputs(integerInputs.begin(), integerInputs.end());

            std::queue<std::shared_ptr<Process>> routine_queue;
            auto routine = simulation_handler->call(0, link_event->source, link_event->comp_id, link_event->target, link_event->op_param, link_event->op_id, floatInputs, link_event->sub_type);
    	    routine_queue.push(routine);

    	    std::shared_ptr<simEvent> routine_event = std::make_shared<subprocessEvent>(routine_queue);
            tick(self_time, routine_event, componentProcess);
                              
            delete link_event;

        }      

    }   

    else 
    {
        throw std::runtime_error("Error! Bad Event Type!\n");//printf("Error! Bad Event Type!\n");
    }

}


void beComponent::handleEvents(std::shared_ptr<simEvent> event, std::shared_ptr<Process> eventProcess){

	std::string event_type = event->type;

	if (event_type == "change"){

             auto ev = std::dynamic_pointer_cast<changeEvent>(event);
             ev->state[ev->attribute] = ev->value;
             simulation_handler->hardware_state[ev->attribute] = ev->value;
             if((simulation_handler->sim_flags)->debug == self_gid) printf("GID: %d -- Handling change event\n", self_gid);
             //if((simulation_handler->sim_flags)->debug == self_gid) std::cout<<"Watchlist length is "<<watchList.size()<<"\n";
             bool process_ready = false;
             std::shared_ptr<conditionEvent> cond_event;
             std::shared_ptr<Process> cond_process;

             for (auto itr = watchList.begin(); itr != watchList.end(); itr++)         //Might have bugs...check!!!
             {         
                 cond_event = std::get<0>(itr->second);
                 cond_process = std::get<1>(itr->second);
                 if(cond_event->conditionFunction(simulation_handler->hardware_state[cond_event->attribute], cond_event->value))
                 {
                     watchList.erase(itr);
                     process_ready = true;
                     //if((simulation_handler->sim_flags)->debug == self_gid) std::cout<<"Executing one waiting routine process from the watchList\n";
                     break;
                 }

             }
             
             if(process_ready == true) tick(self_time, cond_event, cond_process);
          
	}

	else if (event_type == "timeout")
        {
	    if((simulation_handler->sim_flags)->debug == self_gid) printf("GID: %d -- Handling timeout event\n", self_gid);
	}

	else if (event_type == "condition")
        {
	    if((simulation_handler->sim_flags)->debug == self_gid) printf("GID: %d -- Handling condition event\n", self_gid); 
	}

	else if (event_type == "communicate")
        {

            auto comm_event = std::dynamic_pointer_cast<commEvent>(event);
            int next_hop = -1;

            if((simulation_handler->sim_flags)->debug == self_gid) printf("GID: %d -- Forwarding the message to the next component %d\n", self_gid, next_hop);

            /* calculate next hop */ 
            if(self_ordinal != -1)
            {
                int hier_target = (comm_event->tarlist).back();
                int targetOneLevelDown = -1;

                if((comm_event->tarlist).size() > 1) targetOneLevelDown = (comm_event->tarlist).end()[-2];

                std::tuple<bool, int, int> route_set = (simulation_handler->dynamicRouter)->findNextHop(hier_target, targetOneLevelDown);

                next_hop = std::get<1>(route_set);

                if(std::get<0>(route_set) == 1) (comm_event->tarlist).push_back(std::get<2>(route_set));

                else if(std::get<0>(route_set) == 1) (comm_event->tarlist).pop_back();
            }

            else
            {
                int other_end = comm_event->locations[(comm_event->locations).size() - 2];
                 
                for(auto itr = C_Link.begin(); itr != C_Link.end(); itr++) {
                    if(itr->first == other_end) continue;
                    else next_hop = itr->first;
                }
            }

            beCommEvent *bev;
            bev = buildLinkEvent(comm_event->type, comm_event->source, comm_event->target, comm_event->tarlist, comm_event->locations, comm_event->pid, 
                                 std::to_string(comm_event->size), std::to_string(comm_event->tag), comm_event->comm_type);

            if(next_hop >= 0) C_Link[next_hop]->send(bev);
                
	}

	else if (event_type == "initiate communication")
        {

	    if((simulation_handler->sim_flags)->debug == self_gid) printf("GID: %d -- Handling communication initiation\n", self_gid);

            auto messageProcess = std::dynamic_pointer_cast<Message>(eventProcess);
            auto comm_event = std::dynamic_pointer_cast<commEvent>(event);

            (comm_event->tarlist).push_back(comm_event->target);

            beCommEvent *bev;
            bev = buildLinkEvent("communicate", comm_event->source, comm_event->target, comm_event->tarlist, comm_event->locations, comm_event->pid, 
                                 std::to_string(comm_event->size), std::to_string(comm_event->tag), comm_event->comm_type);

            selfEventLink->send(0.000000718, bev);
                
	}

	else if (event_type == "call"){

                auto call_event = std::dynamic_pointer_cast<callEvent>(event);

                std::vector<float> floatInputs = call_event->inputs;
                std::vector<int> integerInputs(floatInputs.begin(), floatInputs.end());
                std::vector<int> empty;

                beCommEvent *bev;
                bev = buildLinkEvent(call_event->type, call_event->source_gid, call_event->target_gid, empty, integerInputs,  
                                     call_event->source_pid, call_event->target_family, call_event->operation, call_event->call_type);

                int target = call_event->target_gid;

                if(target >= 0) C_Link[target]->send(bev);
                if((simulation_handler->sim_flags)->debug == self_gid) std::cout<<"GID: "<<self_gid<<" -- Calling the component "<<target<<"\n";
                
	}

	else if (event_type == "acknowledge"){

                auto ack_ev = std::dynamic_pointer_cast<ackEvent>(event);
                std::vector<int> locations = ack_ev->locations;
                std::vector<int> empty;
                int pid = ack_ev->pid;
                int nxt_hop = ack_ev->next_hop;

                beCommEvent *bev;
                bev = buildLinkEvent("acknowledge", -1, -1, empty, locations, pid, "1", "", "");

                if((simulation_handler->sim_flags)->debug == self_gid) std::cout<<"GID: "<<self_gid<<" -- Sending ack to "<<nxt_hop<<" from "<<self_gid<<"\n";
                if(nxt_hop == self_gid) selfEventLink->send(bev);
                else if(nxt_hop >= 0) C_Link[nxt_hop]->send(bev);
                
	}

	else if (event_type == "subprocess"){

		auto ev = std::dynamic_pointer_cast<subprocessEvent>(event);
                if((simulation_handler->sim_flags)->debug == self_gid) printf("GID: %d -- Handling subprocess event\n", self_gid);
		while (!ev->processes.empty()){                      
			eventProcess->append(ev->processes.front()); //what if there are more than one routines in the sub process event 		
			step(ev->processes.front());
			ev->processes.pop(); 
		}

	}

	else if (event_type == "autoprocess"){

		auto ev = std::dynamic_pointer_cast<autoprocessEvent>(event);
                if((simulation_handler->sim_flags)->debug == self_gid) printf("GID: %d -- Handling autoprocess event\n", self_gid);
		while (!ev->processes.empty()){
			componentProcess->append(ev->processes.front());
			step(ev->processes.front());
			ev->processes.pop();
		}
	}

	else if (event_type == "barrier"){
	    if((simulation_handler->sim_flags)->debug == self_gid) printf("GID: %d -- Handling barrier event\n", self_gid);
	}

	else if (event_type == "wait"){
	    if((simulation_handler->sim_flags)->debug == self_gid) printf("GID: %d -- Handling wait event\n", self_gid);
	}

	else if (event_type == "terminate"){
            if(self_cid == 0) std::cout<<(getCurrentSimTimeNano()*0.000000001)<<"\n";
            if((simulation_handler->sim_flags)->debug == self_gid) printf("GID: %d -- Handling terminate event. Component simulation terminating\n", self_gid);
            primaryComponentOKToEndSim();
	}

}


void beComponent::finish()
{
    /*int i, j, k, l, call_i = 0, comm_i = 0, event_number = 0;
    std::ofstream trace_op;
    trace_op.open("event_trace.txt", std::ios::out | std::ios::app);

    for(i = 0; i < call_records.size()+comm_records.size(); i++)
    {

        if(call_i < call_records.size())
        {

            if((std::get<2>(call_records[call_i])[1]) == event_number+1)
            {   
                trace_op<<"("<<std::get<0>(call_records[call_i])<<", '"<<std::get<1>(call_records[call_i])<<"', ["<<(std::get<2>(call_records[call_i])[0])<<", "<<(std::get<2>(call_records[call_i])[1])<<"], '"<<std::get<3>(call_records[call_i])<<"', ["<<(std::get<4>(call_records[call_i])[0])<<", "<<(std::get<4>(call_records[call_i])[1])<<"], '"<<std::get<5>(call_records[call_i])<<"', [";

                std::vector<int> parameters = std::get<6>(call_records[call_i]);

	        for(j = 0; j < parameters.size(); j++){
                    trace_op<<parameters[j];
                    if(j+1 < parameters.size()) trace_op<<", ";
                }

                trace_op<<"])\n";

                call_i++;
                event_number++;
            }
    
        }

        if(comm_i < comm_records.size())
        {

            if(self_gid != std::get<0>(comm_records[comm_i]))
            {

                trace_op<<"("<<std::get<0>(comm_records[comm_i])<<", '"<<std::get<1>(comm_records[comm_i])<<"', ("<<std::get<0>(std::get<2>(comm_records[comm_i]))<<", "<<std::get<2>(std::get<2>(comm_records[comm_i]))<<"), "<<"("<<std::get<0>(std::get<3>(comm_records[comm_i]))<<", "<<std::get<1>(std::get<3>(comm_records[comm_i]))<<"), "<<"[";

                std::vector< std::tuple<int, std::tuple<double, double>, std::string, std::vector<int>, std::vector<int>> > messagePath = std::get<4>(comm_records[comm_i]);

                for(k = 0; k < messagePath.size(); k++)
                {
                    trace_op<<"("<<std::get<0>(messagePath[k])<<", ("<<std::get<0>(std::get<1>(messagePath[k]))<<", "<<std::get<1>(std::get<1>(messagePath[k]))<<"), '"<<std::get<2>(messagePath[k])<<"', [";
                    std::vector<int> mparameters = std::get<3>(messagePath[k]);

	            for(j = 0; j < mparameters.size(); j++){
                        trace_op<<mparameters[j];
                        if(j+1 < mparameters.size()) trace_op<<", ";
                    }

                    std::vector<int> mlocations = std::get<4>(messagePath[k]);
                    trace_op<<"], [";

                    for(l = 0; l < mlocations.size(); l++){
                        trace_op<<mlocations[l];
                        if(l+1 < mlocations.size()) trace_op<<", ";
                    }

                    if(k+1 < messagePath.size()) trace_op<<"]), ";
                }   

                trace_op<<"])])\n";

                comm_i++;
            }   

            else if(std::get<2>(std::get<2>(comm_records[comm_i])) == event_number+1)
            {
                trace_op<<"("<<std::get<0>(comm_records[comm_i])<<", '"<<std::get<1>(comm_records[comm_i])<<"', ("<<std::get<0>(std::get<2>(comm_records[comm_i]))<<", "<<std::get<2>(std::get<2>(comm_records[comm_i]))<<"), "<<"("<<std::get<0>(std::get<3>(comm_records[comm_i]))<<", "<<std::get<1>(std::get<3>(comm_records[comm_i]))<<"), "<<"[";

                std::vector< std::tuple<int, std::tuple<double, double>, std::string, std::vector<int>, std::vector<int>> > messagePath = std::get<4>(comm_records[comm_i]);

                for(k = 0; k < messagePath.size(); k++)
                {
                    trace_op<<"("<<std::get<0>(messagePath[k])<<", ("<<std::get<0>(std::get<1>(messagePath[k]))<<", "<<std::get<1>(std::get<1>(messagePath[k]))<<"), '"<<std::get<2>(messagePath[k])<<"', [";
                    std::vector<int> mparameters = std::get<3>(messagePath[k]);

	            for(j = 0; j < mparameters.size(); j++){
                        trace_op<<mparameters[j];
                        if(j+1 < mparameters.size()) trace_op<<", ";
                    }

                    std::vector<int> mlocations = std::get<4>(messagePath[k]);
                    trace_op<<"], [";

                    for(l = 0; l < mlocations.size(); l++){
                        trace_op<<mlocations[l];
                        if(l+1 < mlocations.size()) trace_op<<", ";
                    }

                    if(k+1 < messagePath.size()) trace_op<<"]), ";
                }   

                trace_op<<"])])\n";

                comm_i++;
                event_number++;
            }
        }

    }

    trace_op.close();
   */         
    Py_Finalize();
    //if(!waitQ.empty()) {printf("%d waiting", self_gid); std::cout<<" "<<self_kind<<"\n";}
    //if(!watchList.empty()) std::cout<<self_gid<<"--"<<self_kind<<" is pending on unsatisfied condition\n";
    //printf("Inside finish for %d\n", self_gid);
   
}
