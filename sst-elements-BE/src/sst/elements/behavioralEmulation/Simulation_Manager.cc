#include <signal.h>
#include "Simulation_Manager.h"

using namespace SST;
using namespace SST::BEComponent;


std::tuple<std::vector<double>, std::queue<std::shared_ptr<eventTemplate>>> simManager::lookup(std::string operation, std::vector<float> inputs)
{

    std::queue<std::shared_ptr<eventTemplate>> templates;
    bool present = false;
    std::vector<double> outputs;
    std::string lookup_file, interpolation;

    if(operations.find(operation) != operations.end()) std::tie(lookup_file, interpolation, templates) = operations[operation];
  
    else {
        std::cout<<"Error accessing details of Operation "<<operation<<"\n";
        throw std::runtime_error("Error accessing details of Operation "+operation+"\n");
    }
  
    if(lookup_file != "None")
    { 
        /*if(self_gid == 7) {
            for(int j =0; j<inputs.size(); j++) std::cout<<inputs[j]<<" ";
            printf("\n");
        }*/

        lookup_cache = getCache();
    
        for(auto itr = lookup_cache.begin(); itr != lookup_cache.end(); itr++)
        {
            if(std::get<0>(itr->first) == lookup_file && vectorCheck(inputs, std::get<1>(itr->first))) {
                outputs = itr->second;
                present = true;
                break;
            }    
        }
      
        if(!present)
        {
            PyObject* inputList = vectorToList(inputs);
            PyObject* filename = PyString_FromString(lookup_file.c_str());
            PyObject* i_scheme;

            if(interpolation == "None" || interpolation == "default") i_scheme = PyString_FromString((sim_flags->interpolation_scheme).c_str());
            else i_scheme = PyString_FromString(interpolation.c_str());

            PyObject* outputList = PyObject_CallFunctionObjArgs(myFunction, filename, inputList, i_scheme, NULL);
                //PyObject* myValue = PyList_GetItem(outputList, 0);
                //double result = PyFloat_AsDouble(myValue);
            outputs = listToVector(outputList);
            lookup_cache[make_tuple(lookup_file, inputs)] = outputs;
            updateCache(lookup_cache);
        }
      
    } 

    return std::make_tuple (outputs, templates);

}


PyObject* simManager::vectorToList(std::vector<float> data){

  PyObject* listObj = PyList_New( data.size() );
	if (!listObj) throw std::runtime_error("Unable to allocate memory for Python list");
	for (int i = 0; i < data.size(); i++) {
		PyObject *num = PyFloat_FromDouble( (double) data[i]);
		if (!num) {
			Py_DECREF(listObj);
			throw std::runtime_error("Unable to allocate memory for Python list");
		}
		PyList_SET_ITEM(listObj, i, num);
	}
	return listObj;
}


std::vector<double> simManager::listToVector(PyObject* incoming){

	std::vector<double> data;

	if(PyList_Check(incoming))
	{
	    for(Py_ssize_t i = 0; i < PyList_Size(incoming); i++){
				PyObject *value = PyList_GetItem(incoming, i);
				data.push_back(PyFloat_AsDouble(value));
		}
	} 
	else
	{
		throw std::runtime_error("Passed PyObject pointer was not a list!");
		data.push_back(0.0);
	}
		
	return data;
	
}


bool simManager::vectorCheck(std::vector<float> list1, std::vector<float> list2){

    if(list1.size() == list2.size())
    {
        for(int i = 0; i < list1.size(); i++)
            if(list1[i] != list2[i]) return false;
    }
    else
        return false;
        
    return true;
    
}


int simManager::obtain(std::string property){  //generalised implementation reqd

    try
    {
        if(property == "sim.time") return 0;//currentSimTime();
        if(properties[property] == "True") return 1;
        else if(properties[property] == "False") return 0;
        else return stoi(properties[property]);
    }
    catch(...)
    {
        std::cout<<"Problem with the Property: "<<property<<"\n";
        throw std::runtime_error("Problem with the Property");       
    }

}
        

float simManager::fobtain(std::string property){  //generalised implementation reqd

    try
    {
        if(property == "sim.time") return 0.0;//currentSimTime();
        if(properties[property] == "True") return 1.0;
        else if(properties[property] == "False") return 0.0;
        else return stod(properties[property]);
    }
    catch(...)
    {
        std::cout<<"Problem with the Property: "<<property<<"\n";
        throw std::runtime_error("Problem with the Property");       
    }

}


std::shared_ptr<Routine> simManager::call(int eventId, int source_gid, int source_pid, int target_gid, std::string target, std::string operation, std::vector<float> inputs, std::string call_type){

    int targetGID;
    if(self_gid == source_gid) targetGID = relations[target];
    else targetGID = target_gid;
    std::vector<double> outputs;
    std::queue<std::shared_ptr<eventTemplate>> templates;

    if(targetGID == self_gid)
    {
        std::tie(outputs, templates) = lookup(operation, inputs);
        
        if(source_gid != self_gid && call_type == "blocking")
        {
            //std::cout<<"Call being serviced by "<<self_gid<<" for "<<source_gid;
            std::vector<int> sourcePath;
            sourcePath.push_back(self_gid);
            sourcePath.push_back(source_gid);
            templates.push(std::make_shared<ackTemplate>(source_pid, sourcePath, source_gid, true));
        }

        auto routine = std::make_shared<Routine>(source_gid, eventId, hardware_state, inputs, outputs, templates, "call");  //source_gid instead of targetGid because the owner of call is the source
        return routine;
    }

    else
    {
        templates.push(std::make_shared<callTemplate>(self_gid, source_pid, targetGID, target, operation, inputs, call_type, true));
        if(call_type == "blocking") templates.push(std::make_shared<waitTemplate>(true));
        auto routine = std::make_shared<Routine>(self_gid, eventId, hardware_state, inputs, outputs, templates, "call");  //previously it was targetGID instead of self_gid. Check if self_gid is okay
        return routine;
    }

}


std::shared_ptr<Message> simManager::comm(int gid, int pid, int eventId, std::string operation, int size, int target_rank, int tag, std::string comm_type){

    int source = self_ordinal; //layout->rordinals[executor->gid];
    //std::cout<<"Source: "<<source<<" and Target: "<<target_rank<<"\n";
    if (operation == "send")
    {
        std::vector<int> locations;// = router->path(source, target_rank);
        //router->clear();
        auto message = std::make_shared<Message>(gid, pid, eventId, source, target_rank, size, tag, locations, comm_type, hardware_state);

        //std::cout<<"Locations generated are ";
            
        return message;
    }

    else return NULL;

}


std::queue<std::shared_ptr<Process>> simManager::dynamic_mailbox_routines (std::vector<int> locations, std::vector<int> tarlist, int pid, int source, int target, int size, int tag, std::string comm_type, bool isDestination)
{

    std::string operation;
    bool onSource, onMiddle, onTarget; 
    std::vector<float> inputs;
    std::vector<double> outputs;
    std::queue<std::shared_ptr<eventTemplate>> templates;
    std::queue<std::shared_ptr<Process>> routines;

    std::vector<int> rlocations = locations;
    std::reverse(rlocations.begin(), rlocations.end());

    std::tuple<bool, bool, bool> targets;
 
    std::tie(operation, targets) = mailboxes; // list of multiple operations? or just one per kind 
    std::tie(onSource, onMiddle, onTarget) = targets;

    if( (self_gid == locations[0] && onSource) || (self_gid != locations[0] && onMiddle) || (isDestination && onTarget) ) 
    {
        inputs = mailbox_function(source, target, size, tag);   
        std::tie(outputs, templates) = lookup(operation, inputs);
      
        if(!isDestination)
            templates.push(std::make_shared<communicateTemplate>(pid, source, size, target, tag, tarlist, locations, -1, comm_type, true));
        else if(comm_type == "blocking"){
            templates.push(std::make_shared<ackTemplate>(pid, rlocations, rlocations[0], true));
        }

	auto routine = std::make_shared<Routine>(locations[0], pid, hardware_state, inputs, outputs, templates, "comm");
        routines.push(routine);		 
    }

    return routines;

}


std::queue<std::shared_ptr<Process>> simManager::mailbox_routines (std::vector<int> gids, int pid, int source, int target, int size, int tag, std::string comm_type)
{

    std::vector<int>::iterator igid;
    std::vector<int> locations = gids;
    int i;

    std::vector<int> empty;
    std::vector<int> rlocations = locations;
    std::reverse(rlocations.begin(), rlocations.end());

    std::string operation;

    bool onSource, onMiddle, onTarget;
 
    std::vector<float> inputs;
    std::vector<double> outputs;

    std::queue<std::shared_ptr<eventTemplate>> templates;
    std::queue<std::shared_ptr<Process>> routines;

    for (i = 0, igid = gids.begin(); igid != gids.end(); ++i, ++igid)
    {
        if((*igid) == self_gid)
        {  
            std::tuple<bool, bool, bool> targets;
 
            std::tie(operation, targets) = mailboxes; // list of multiple operations? or just one per kind 
            std::tie(onSource, onMiddle, onTarget) = targets;

            if ((i == 0 && onSource) || (i == gids.size()-1 && onTarget) || (i < gids.size()-1 && onMiddle))
            {
                inputs = mailbox_function(source, target, size, tag);   
		std::tie(outputs, templates) = lookup(operation, inputs);
      
                if(i != gids.size()-1)
                    templates.push(std::make_shared<communicateTemplate>(pid, source, size, target, tag, empty, locations, locations[i+1], comm_type, true));
                else if(comm_type == "blocking")
                    templates.push(std::make_shared<ackTemplate>(pid, rlocations, rlocations[0], true));

		auto routine = std::make_shared<Routine>(locations[0], pid, hardware_state, inputs, outputs, templates, "comm");
                routines.push(routine);		 
	    }
        }
 
    }
 
    return routines;

}


std::queue<std::shared_ptr<Process>> simManager::ack_routines (std::vector<int> gids, int pid, int source, int target, int size, int tag, std::string comm_type)
{

    std::vector<int>::iterator igid;
    std::vector<int> locations = gids;
    int i;
    std::string operation;
    bool onSource, onMiddle, onTarget; 
    std::vector<float> inputs;
    std::vector<double> outputs;
    std::queue<std::shared_ptr<eventTemplate>> templates;
    std::queue<std::shared_ptr<Process>> routines;

    for (i = 0, igid = gids.begin(); igid != gids.end(); ++i, ++igid)
    {
        if((*igid) == self_gid)
        {  
            std::tuple<bool, bool, bool> targets;
 
            std::tie(operation, targets) = mailboxes; // list of multiple operations? or just one per kind 
            std::tie(onSource, onMiddle, onTarget) = targets;

            if ((i == 0 && onSource) || (i == gids.size()-1 && onTarget) || (i < gids.size()-1 && onMiddle))
            {
                inputs.push_back(size);  
		std::tie(outputs, templates) = lookup(operation, inputs);
      
                if(i != gids.size()-1)
                    templates.push(std::make_shared<ackTemplate>(pid, locations, locations[i+1], true));

		auto routine = std::make_shared<Routine>(locations.back(), pid, hardware_state, inputs, outputs, templates, "ack");
                routines.push(routine);		 
	    }
        }
 
    }
 
    return routines;

}


std::shared_ptr<Executor> simManager::prog(std::string filename) //check if correct
{

    auto executor = std::make_shared<Executor>(self_gid, giveExecutorId(), filename, std::shared_ptr<simManager>(this), hardware_state);
    return executor;
}


std::vector<std::string> simManager::decode(std::string operand, std::string delimiter)
{	

    size_t pos = 0;
    std::vector<std::string> operandList;
    ////std::cout<<operand<<std::endl;
    while((pos = operand.find(delimiter)) != std::string::npos){          
        ////std::cout<<operand<<std::endl;  
        operandList.push_back(operand.substr(0, pos));
        ////std::cout << operand.substr(0, pos) << std::endl;
        operand.erase(0, pos + delimiter.length());
    }
    operandList.push_back(operand);

    return operandList;
            
}


void simManager::buildInformation(std::string templates, std::string relation_info, std::string property_info, std::string mailbox_info,
                                  std::string container_dimensions, std::string plus_list, std::string minus_list, std::string ch_list, std::string s_flags)
{

    std::vector<std::string> list1, list2, list3, list4;

    /* Parse the template string to build the set of operations as specified in the configuration file*/
    if(templates.size() > 4)
        list1 = decode(templates.substr(1, templates.size()-4), "]), ");

    for(auto itr = list1.begin(); itr != list1.end(); itr++)
    {
        //std::cout<<"itr: "<<*itr<<"\n";
        list2 = decode(*itr, ": (");
        std::string opr, oprList, lookup_s, events_s, interpolation_s;
 
        if(list2.size() == 2){
            opr = list2[0];
            oprList = list2[1];
        }

        list3 = decode(oprList, ", [");

        if(list3.size() == 2){
            std::vector<std::string> lookupInfo;
            lookupInfo = decode(list3[0], ", "); 
            lookup_s = lookupInfo[0];
            if(lookupInfo.size() == 2) interpolation_s = lookupInfo[1];
            events_s = list3[1];
        }

        list4 = decode(events_s, ", ");

        std::queue<std::shared_ptr<eventTemplate>> op_template;

        for(auto itr1 = list4.begin(); itr1 != list4.end(); itr1++)
        {
            //std::cout<<"itr1: "<<*itr1<<"\n";
            std::vector<std::string> list5;

            if(itr1->size() > 2)
                list5 = decode(itr1->substr(1, itr1->size()-2), " ");

            op_template.push(buildTemplate(list5));

        }
        
        if(lookup_s != "None" && lookup_s.size() > 2) lookup_s = lookup_s.substr(1, lookup_s.size()-2);
        if(interpolation_s != "None" && interpolation_s.size() > 2) interpolation_s = interpolation_s.substr(1, interpolation_s.size()-2);
        
        operations[opr.substr(1, opr.size()-2)] = std::tie(lookup_s, interpolation_s, op_template);

    }

    
    std::vector<std::string> empty1, empty2, empty3, empty4;

    /* Parse the relation string to build the component relations as specified in the configuration file */
    std::swap(list1, empty1);

    if(relation_info.size() > 2)
        list1 = decode(relation_info.substr(1, relation_info.size()-2), ", "); 

    for(auto itr = list1.begin(); itr != list1.end(); itr++)
    {
        list2 = decode(*itr, ": ");   

        if(list2.size() == 2)
            relations[list2[0].substr(1, list2[0].size()-2)] = stoi(list2[1]);
    }

    
    /* Parse the property string to build the set of properties for the component as specified in the configuration file */
    std::swap(list1, empty2);

    if(property_info.size() > 2)
        list1 = decode(property_info.substr(1, property_info.size()-2), ", "); 

    for(auto itr = list1.begin(); itr != list1.end(); itr++)
    {
        list2 = decode(*itr, ": ");   

        if(list2.size() == 2)
            properties[list2[0].substr(1, list2[0].size()-2)] = list2[1];
    }


    /* Parse the mailbox string in order to build the component mailbox (set of operations and its parameters that need to be invoked in case of a message receive) */
    /* Assumes a single mailbox per component for now */
    std::swap(list1, empty3);
 
    std::string mailbox_operation;
    std::tuple<bool, bool, bool> mailbox_targets;

    if(mailbox_info.size() > 2)
        list1 = decode(mailbox_info.substr(2, mailbox_info.size()-5), ", [");

    if(list1.size() == 2) 
    {
        mailbox_operation = list1[0].substr(1, list1[0].size()-2);       
    
        list2 = decode(list1[1], "], (");

        if(list2.size() == 2) {
            mailboxList = decode(list2[0], ", ");
            list4 = decode(list2[1], ", ");
            bool s = stringToBool(list4[0]), m = stringToBool(list4[1]), d = stringToBool(list4[2]);
            mailbox_targets = std::tie(s, m, d);
        }
    }

    mailboxes = std::tie(mailbox_operation, mailbox_targets);


    /* Parse the network dimensions info of the parent container, positive axis neighbour and negative axis neighbour info */
    std::vector<std::string> plus_linklist;
    std::vector<std::string> minus_linklist;
    std::vector<std::string> dimension_list;
    std::vector<std::string> children_list;

    plus_linklist = decode(plus_list.substr(1, plus_list.size()-2), ", ");
    minus_linklist = decode(minus_list.substr(1, minus_list.size()-2), ", ");
    dimension_list = decode(container_dimensions.substr(1, container_dimensions.size()-2), ", ");
    children_list = decode(ch_list.substr(1, ch_list.size()-2), ", ");

    int i;
    for(i = 0; i < dimension_list.size(); i++) {
        if(dimension_list[i] != "") containerDimensions.push_back(stoi(dimension_list[i]));
    }

    for(i = 0; i < plus_linklist.size(); i++) {
        if(plus_linklist[i] != "") plusNeighbour.push_back(stoi(plus_linklist[i]));
    }

    for(i = 0; i < minus_linklist.size(); i++) { 
        if(minus_linklist[i] != "") minusNeighbour.push_back(stoi(minus_linklist[i]));
    }

    for(i = 0; i < children_list.size(); i++) { 
        if(children_list[i] != "") children.push_back(stoi(children_list[i]));
    }


    /* Parse the argument flag information given in the command line */
    std::swap(list1, empty4);

    if(s_flags.size() > 2)
        list1 = decode(s_flags.substr(1, s_flags.size()-2), ", "); 

    for(auto itr = list1.begin(); itr != list1.end(); itr++)
    {
        list2 = decode(*itr, ": ");   

        std::string flag, flag_value;
        if(list2.size() == 2){
            flag = list2[0].substr(1, list2[0].size()-2);
            flag_value = list2[1].substr(1, list2[1].size()-2);
        }

        if(flag == "interpolation" && flag_value != "") sim_flags->interpolation_scheme = flag_value;
   
        else if(flag == "output_file" && flag_value != "") sim_flags->output_file = flag_value;

        else if(flag == "debug" && flag_value != "") {

            if(flag_value == "all") sim_flags->debug = self_gid;

            else {
                try{
                    sim_flags->debug = stoi(flag_value);
                }
                catch(...){
                    throw std::runtime_error("Debug flag specified has a non integer argument other than 'all'!\n");
                }
            }
        }

        else if(flag == "eventtrace" && flag_value != "") sim_flags->traceOutput = flag_value;

    }

}


std::shared_ptr<eventTemplate> simManager::buildTemplate(std::vector<std::string> t_list)
{

    bool prov;
    std::string type;
    std::vector<std::string> list1;
    Procrastinator* container;

    if(t_list.back() == "True") prov = true;
    else prov = false;

    if(t_list[0] == "condition")
        if(t_list[3] == "True") t_list[3] = "1.0";
        else if(t_list[3] == "False") t_list[3] = "0.0";

    if(t_list[0] == "change")                  
        if(t_list[2] == "True") t_list[2] = "1.0";
        else if(t_list[2] == "False") t_list[2] = "0.0";

    
    if(t_list[0] == "condition") 
    {        
        list1 = decode(t_list[3], "::");
        if(!list1.empty()) std::tie(type, container) = value_find(list1);

        if(type == "double") return std::make_shared<conditionTemplate<double>>(t_list[1], t_list[2], stod(t_list[3]), "double", prov);
        else if(type == "procrastinator") return std::make_shared<conditionTemplate<Procrastinator*>>(t_list[1], t_list[2], container, "procrastinator", prov);
    }

    else if(t_list[0] == "change") 
    {
        list1 = decode(t_list[2], "::");
        if(!list1.empty()) std::tie(type, container) = value_find(list1);

        if(type == "double") return std::make_shared<changeTemplate<double>>(t_list[1], stod(t_list[2]), "double", prov);
        else if(type == "procrastinator") return std::make_shared<changeTemplate<Procrastinator*>>(t_list[1], container, "procrastinator", prov);
    }

    else if(t_list[0] == "timeout") 
    {
        list1 = decode(t_list[1], "::");
        if(!list1.empty()) std::tie(type, container) = value_find(list1);

        if(type == "double") return std::make_shared<timeoutTemplate<double>>(stod(t_list[1]), "double", prov);
        else if(type == "procrastinator") return std::make_shared<timeoutTemplate<Procrastinator*>>(container, "procrastinator", prov);
    }

    else if(t_list[0] == "receivewait")
    {
        return std::make_shared<recvWaitTemplate>(prov);
    }

    else if(t_list[0] == "receive")
    {
        return std::make_shared<recvTemplate>(prov);
    } 

    else return std::make_shared<timeoutTemplate<double>>(0, "none", false);

    return std::make_shared<timeoutTemplate<double>>(0, "none", false);

}


std::tuple<std::string, Procrastinator*> simManager::value_find(std::vector<std::string> list)
{
    try
    {
        double d = stod(list[0]);
        Procrastinator *p = NULL;
        return std::tie("double", p);
    }

    catch(...)
    {
       
        std::string math_operation;
        std::tuple<std::string, Procrastinator*> operandValue;

        if(list.size() > 2)
        {
            std::vector<std::string> subList = list;
            subList.erase(subList.begin(), subList.begin()+2); // remove the first 2 elements of the list - main procrastinator and the first operation
            operandValue = value_find(subList);  
            //if(self_gid == 42) std::cout<<std::get<0>(operandValue)<<" "<<std::get<1>(operandValue)->type<<"\n";          
            math_operation = list[1];
        } 

        std::vector<std::string> list1 = decode(list[0], "(");
        
        if(list1[0] == "Attribute"){
            attributeProcrastinator *a = new attributeProcrastinator(list1[1].substr(0, list1[1].size()-1));
            if(std::get<0>(operandValue) == "double") a = dynamic_cast<attributeProcrastinator *>(a->operate(math_operation, stod(list[2]), std::get<1>(operandValue)));
            else if(std::get<0>(operandValue) == "procrastinator") a = dynamic_cast<attributeProcrastinator *>(a->operate(math_operation, 0.0, std::get<1>(operandValue)));
            return std::tie("procrastinator", a);
        }
        else if(list1[0] == "Input"){
            inputProcrastinator *i = new inputProcrastinator(stod(list1[1].substr(0, list1[1].size()-1)));
            if(std::get<0>(operandValue) == "double") i = dynamic_cast<inputProcrastinator *>(i->operate(math_operation, stod(list[2]), std::get<1>(operandValue)));
            else if(std::get<0>(operandValue) == "procrastinator") i = dynamic_cast<inputProcrastinator *>(i->operate(math_operation, 0.0, std::get<1>(operandValue)));
            return std::tie("procrastinator", i);
        }
        else if(list1[0] == "Output"){
            outputProcrastinator *o = new outputProcrastinator(stod(list1[1].substr(0, list1[1].size()-1)));
            if(std::get<0>(operandValue) == "double") o = dynamic_cast<outputProcrastinator *>(o->operate(math_operation, stod(list[2]), std::get<1>(operandValue)));
            else if(std::get<0>(operandValue) == "procrastinator") o = dynamic_cast<outputProcrastinator *>(o->operate(math_operation, 0.0, std::get<1>(operandValue)));
            return std::tie("procrastinator", o);
        }
        else if(list1[0] == "Random"){
            randomProcrastinator *r = new randomProcrastinator(0);
            if(std::get<0>(operandValue) == "double") r = dynamic_cast<randomProcrastinator *>(r->operate(math_operation, stod(list[2]), std::get<1>(operandValue)));
            else if(std::get<0>(operandValue) == "procrastinator") r = dynamic_cast<randomProcrastinator *>(r->operate(math_operation, 0.0, std::get<1>(operandValue)));
            return std::tie("procrastinator", r);
        }
        else{
            Procrastinator *p = NULL;
            return std::tie("none", p);
        }

    }

}
