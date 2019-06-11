#include <signal.h>
#include <fstream>
#include "Executor.h"
#include "Routine.h"
#include <string>

using namespace SST;
using namespace SST::BEComponent;


std::shared_ptr<simEvent> Executor::run()
{

    FuncP operation;    
    std::shared_ptr<simEvent> event;

    while (instruction != program.end()){
        
        if((simulation_handler->sim_flags)->debug == gid) std::cout << "Software for "<< gid << "-> Instruction: " << instruction->kind << " line: " << instruction->line << std::endl;

        if (instruction->kind == "call"){
            event = call(instruction->operands);                 
        }
        else if (instruction->kind == "comm"){
            //printf("Inside this\n");
            event = comm(instruction->operands.begin());            
        }
        else if (instruction->kind == "prog"){
            event = prog(instruction->operands.begin());               
        }
        else if (instruction->kind == "begin"){
            event = begin(instruction->operands.begin());
        }
        else if (instruction->kind == "barrier"){
	    event = barrier(instruction->operands.begin());
        }
        else{
    	    operation = inst_map[instruction->kind];
            (this->*operation)(instruction->operands);
			            
        }
        
       instruction++;
       if (event)
         return event;
     } 
    
    return NULL;
}


void Executor::createImap()
{
    //printf("Inside Imap \n");
    /* May require support for fjump, fadd, fsub, fmul, fdiv and its variants */
    inst_map.insert(std::make_pair("indep", &Executor::indep));
    inst_map.insert(std::make_pair("simul", &Executor::simul));
    inst_map.insert(std::make_pair("print", &Executor::print));
    inst_map.insert(std::make_pair("obtain", &Executor::obtain));
    inst_map.insert(std::make_pair("fobtain", &Executor::fobtain));
    inst_map.insert(std::make_pair("assign", &Executor::assign));
    inst_map.insert(std::make_pair("access", &Executor::access));
    inst_map.insert(std::make_pair("target", &Executor::target));
    inst_map.insert(std::make_pair("jumpeq", &Executor::jumpeq));
    inst_map.insert(std::make_pair("jumpnq", &Executor::jumpnq));
    inst_map.insert(std::make_pair("jumpgt", &Executor::jumpgt));
    inst_map.insert(std::make_pair("jumplt", &Executor::jumplt));
    inst_map.insert(std::make_pair("jumpng", &Executor::jumplt));
    inst_map.insert(std::make_pair("jumpnl", &Executor::jumplt));
    inst_map.insert(std::make_pair("add", &Executor::add));
    inst_map.insert(std::make_pair("sub", &Executor::sub));
    inst_map.insert(std::make_pair("mul", &Executor::mul));
    inst_map.insert(std::make_pair("div", &Executor::div));
    inst_map.insert(std::make_pair("mod", &Executor::mod));
    inst_map.insert(std::make_pair("inc", &Executor::inc));
    inst_map.insert(std::make_pair("dec", &Executor::dec));
}


void Executor::buildProgram(std::string filename)
{

    std::ifstream program_file;

    program_file.open(filename);

    if(!program_file.good()) throw std::runtime_error(filename+" -- PROBLEM LOADING/LOCATING THE COMPILED SOFTWARE CONFIGURATION FILE!!\n");

    for(std::string line; std::getline(program_file, line); ){
 
        size_t pos = 0;
        std::string inst_num="", inst_name="", inst_operands="", substring="";

        pos = line.find(" :: ");

        if(pos != std::string::npos){            
            substring = line.substr(0, pos);
            line.erase(0, pos + 4);
            inst_operands = line;
            inst_num = substring.substr(0, substring.find(" "));
            inst_name = substring.substr(substring.find(" ")+1);
            std::string::iterator end_pos = std::remove(inst_name.begin(), inst_name.end(), ' ');
            inst_name.erase(end_pos, inst_name.end());
            line = "";
            //std::cout<<inst_num<<"--"<<inst_name<<"--"<<inst_operands<<"\n";
            program.push_back(Instruction((inst_num.substr(1, inst_num.size()-2)), inst_name, inst_operands));
            
        }

    }

}


std::shared_ptr<simEvent> Executor::call(std::vector<std::string> op)
{
    //printf("Inside call \n");
    auto itr = op.begin();
    std::string target = *(itr);
    std::string operation = *(++itr);
    std::vector<float> inputs;
	
    std::string opr;
    std::queue<std::shared_ptr<Process>> routine_queue;		 

    for (++itr ; itr != op.end(); ++itr)
    {
	opr = *itr;

        if (opr[0] == 'r') {

            if(registers.find(opr) != registers.end()) inputs.push_back(registers[opr]);

            else if(fregisters.find(opr) != fregisters.end()) inputs.push_back(fregisters[opr]);    //floating point registers

            else throw std::runtime_error("Value for register "+opr+" is not obtained yet!!\n");
        }

	else
	    inputs.push_back(stod(opr));

    }
    
    if (v_indep || v_simul) {
        auto routine = simulation_handler->call(getEventId(), gid, pid, -1, target, operation, inputs, "non-blocking");
   	process_queue.push(routine);
    	return NULL;
    }

    else {
        auto routine = simulation_handler->call(getEventId(), gid, pid, -1, target, operation, inputs, "blocking");
    	routine_queue.push(routine);
        auto subEv = std::make_shared<subprocessEvent>(routine_queue);
    	return subEv;    	
    }

}


std::shared_ptr<simEvent> Executor::comm(std::vector<std::string>::iterator op){  // Might require handling of inputs from floating point registers just like call

	std::string operation = *op;
        std::string size_string = *(++op);
        std::string ranks_string = *(++op);
        std::string tag_string = *(++op);

        int size, ranks, tag;

        std::queue<std::shared_ptr<Process>> message_queue;

	size_string[0]=='r' ? size = registers[size_string] : size = stoi(size_string);
        ranks_string[0]=='r' ? ranks = registers[ranks_string] : ranks = stoi(ranks_string);
        tag_string[0]=='r' ? tag = registers[tag_string] : tag = stoi(tag_string);


	if (v_indep || v_simul){
	  auto message = simulation_handler->comm(gid, pid, getEventId(), operation, size, ranks, tag, "non-blocking");
    	  process_queue.push(message);    	
    	  return NULL;
        }
        else{
    	    auto message = simulation_handler->comm(gid, pid, getEventId(), operation, size, ranks, tag, "blocking");
    	    message_queue.push(message);
            auto subEv = std::make_shared<subprocessEvent>(message_queue);
    	    return subEv;
        }

}

std::shared_ptr<simEvent> Executor::prog(std::vector<std::string>::iterator op) {

    std::string filename = *op;
    std::queue<std::shared_ptr<Process>> executor_queue;
	
    if (v_indep || v_simul){
	auto executor = simulation_handler->prog(filename);
    	process_queue.push(executor);
    	return NULL;
    }
    else{
    	auto executor = simulation_handler->prog(filename);
    	executor_queue.push(executor);  
        auto subEv = std::make_shared<subprocessEvent>(executor_queue);  	
    	return subEv;
    }
}

std::shared_ptr<simEvent> Executor::begin(std::vector<std::string>::iterator op) {

    std::queue<std::shared_ptr<Process>> emptyQ;
    std::shared_ptr<simEvent> event; 
    
    if (v_indep && !v_simul){
        event = std::make_shared<autoprocessEvent>(process_queue);       
        v_indep = false;  
        std::swap(process_queue, emptyQ);       
    }
    else if (v_simul && !v_indep){
        event = std::make_shared<subprocessEvent>(process_queue);
        v_simul = false;  
        std::swap(process_queue, emptyQ);
    }
    else {
        throw std::runtime_error("Begin called without indep or simul or with both is illegal!!\n");        
    }

    return event;
}

std::shared_ptr<simEvent> Executor::barrier(std::vector<std::string>::iterator op) {

    std::string commgroup = *op;

    auto barrEv = std::make_shared<barrierEvent>(gid, registers[commgroup]);
    return barrEv;
}

void Executor::print(std::vector<std::string> op) {    

    std::ofstream output_file;
    output_file.open((simulation_handler->sim_flags)->output_file, std::ios::out | std::ios::app);
    
    for(auto itr = op.begin(); itr != op.end(); itr++)
    {

	if((*itr)[0] == '"')
        {
            std::string statement = *itr;
            size_t pos = 0;

            while((pos = statement.find("/t")) != std::string::npos)
                statement = statement.replace(pos, 2, " ");    

            output_file<<statement.substr(1, statement.size()-2);
        }

        else if((*itr)[0] == 'r')
        {
            output_file<<registers[(*itr)];
        }

    }

    output_file<<"\n";
    output_file.close();

}

void Executor::obtain(std::vector<std::string> op){ 

    auto itr = op.begin();
    std::string reg = *itr;
    std::string property = *(++itr);

    registers[reg] = simulation_handler->obtain(property);
        
}

void Executor::fobtain(std::vector<std::string> op){ 

    auto itr = op.begin();
    std::string reg = *itr;
    std::string property = *(++itr);

    fregisters[reg] = simulation_handler->fobtain(property);
        
}

void Executor::assign(std::vector<std::string> op){

    auto itr = op.begin();
    std::string reg = *itr;
    std::string value_string = *(++itr);

    std::vector<int> value_list;

    if(value_string[0] == '[')
    {
        size_t pos = 0;
        std::string operand = value_string.substr(1, value_string.size()-2);
        while((pos = operand.find(",")) != std::string::npos){
            if(operand.substr(0, pos) != "")          
                value_list.push_back(stoi(operand.substr(0, pos)));
            operand.erase(0, pos+1);
        }
        if(operand != "")
            value_list.push_back(stoi(operand));

        registers_array[reg] = value_list;
  
    }
    else
        registers[reg] = stoi(value_string);        
    
}

void Executor::access(std::vector<std::string> op){

    auto itr = op.begin();
    std::string reg = *itr;
    std::string reg_array = *(++itr);
    std::string value = *(++itr);

    if(value[0] == 'r')
        registers[reg] = registers_array[reg_array][registers[value]];

    else 
        registers[reg] = registers_array[reg_array][stoi(value)];

}

void Executor::target(std::vector<std::string> op){
    ;  //No-Op
}

void Executor::jumpeq(std::vector<std::string> op){

    auto itr = op.begin();
    std::string left = *itr;
    std::string right = *(++itr);
    std::string target = *(++itr);
    target = target.substr(1, target.size()-2);
    int line = -1;

    if (left[0] == 'r' && right[0] == 'r'){
        if (registers[left] == registers[right]){
            line = stoi(target);            
        }               
    }
    else if (left[0] == 'r' && right[0] != 'r'){
        if (registers[left] == stoi(right)){
            line = stoi(target);        
        }
    }
    else if (left[0] != 'r' && right[0] == 'r'){
        if (stoi(left) == registers[right]){
            line = stoi(target);
        }    
    }
    else{
        if (stoi(left) == stoi(right)){
            line = stoi(target);
        }    
    }
    
    instruction = line >= 0 ? program.begin() + line : instruction;
}

void Executor::jumpnq(std::vector<std::string> op){

    auto itr = op.begin();
    std::string left = *itr;
    std::string right = *(++itr);
    std::string target = *(++itr);
    target = target.substr(1, target.size()-2);
    int line = -1;
    
    if (left[0] == 'r' && right[0] == 'r'){
        if (registers[left] != registers[right]){
            line = stoi(target);
        }        
    }
    else if (left[0] == 'r' && right[0] != 'r'){
        if (registers[left] != stoi(right)){
            line = stoi(target);        
        }
    }
    else if (left[0] != 'r' && right[0] == 'r'){        
        if (stoi(left) != registers[right]){             
            line = stoi(target);
        }         
    }
    else{
        if (stoi(left) != stoi(right)){
            line = stoi(target);
        }    
    }
    
    instruction = line >= 0 ? program.begin() + line : instruction;
}

void Executor::jumpgt(std::vector<std::string> op){

    auto itr = op.begin();
    std::string left = *itr;
    std::string right = *(++itr);
    std::string target = *(++itr);
    target = target.substr(1, target.size()-2);
    int line = -1;

    if (left[0] == 'r' && right[0] == 'r'){
        if (registers[left] > registers[right]){
            line = stoi(target);
        }        
    }
    else if (left[0] == 'r' && right[0] != 'r'){
        if (registers[left] > stoi(right)){
            line = stoi(target);        
        }
    }
    else if (left[0] != 'r' && right[0] == 'r'){
        if (stoi(left) > registers[right]){
            line = stoi(target);
        }    
    }
    else{
        if (stoi(left) > stoi(right)){
            line = stoi(target);
        }    
    }
    
    instruction = line >= 0 ? program.begin() + line : instruction;
}

void Executor::jumplt(std::vector<std::string> op){

    auto itr = op.begin();
    std::string left = *itr;
    std::string right = *(++itr);
    std::string target = *(++itr);
    target = target.substr(1, target.size()-2);
    int line = -1;

    if (left[0] == 'r' && right[0] == 'r'){
        if (registers[left] < registers[right]){
            line = stoi(target);
        }        
    }
    else if (left[0] == 'r' && right[0] != 'r'){
        if (registers[left] < stoi(right)){
            line = stoi(target);        
        }
    }
    else if (left[0] != 'r' && right[0] == 'r'){
        if (stoi(left) < registers[right]){
            line = stoi(target);
        }    
    }
    else{
        if (stoi(left) < stoi(right)){
            line = stoi(target);
        }    
    }
    
    instruction = line >= 0 ? program.begin() + line : instruction;
}

void Executor::jumpng(std::vector<std::string> op){

    auto itr = op.begin();
    std::string left = *itr;
    std::string right = *(++itr);
    std::string target = *(++itr);
    target = target.substr(1, target.size()-2);
    int line = -1;
    
    if (left[0] == 'r' && right[0] == 'r'){
        if (registers[left] <= registers[right]){
            line = stoi(target);
        }        
    }
    else if (left[0] == 'r' && right[0] != 'r'){
        if (registers[left] <= stoi(right)){
            line = stoi(target);        
        }
    }
    else if (left[0] != 'r' && right[0] == 'r'){
        if (stoi(left) <= registers[right]){
            line = stoi(target);
        }    
    }
    else{
        if (stoi(left) <= stoi(right)){
            line = stoi(target);
        }    
    }
    
    instruction = line >= 0 ? program.begin() + line : instruction;
}

void Executor::jumpnl(std::vector<std::string> op){

    auto itr = op.begin();
    std::string left = *itr;
    std::string right = *(++itr);
    std::string target = *(++itr);
    target = target.substr(1, target.size()-2);
    int line = -1;

    if (left[0] == 'r' && right[0] == 'r'){
        if (registers[left] >= registers[right]){
            line = stoi(target);
        }        
    }
    else if (left[0] == 'r' && right[0] != 'r'){
        if (registers[left] >= stoi(right)){
            line = stoi(target);        
        }
    }
    else if (left[0] != 'r' && right[0] == 'r'){
        if (stoi(left) >= registers[right]){
            line = stoi(target);
        }    
    }
    else{
        if (stoi(left) >= stoi(right)){
            line = stoi(target);
        }    
    }
    
    instruction = line >= 0 ? program.begin() + line : instruction;
}

void Executor::add(std::vector<std::string> op){

    auto itr = op.begin();
    std::string target = *itr;
    std::string a = *(++itr);
    std::string b = *(++itr);    
                
    if (a[0] == 'r' && b[0] == 'r'){
        registers[target] = registers[a] + registers[b];   
    }
    else if (a[0] == 'r' && b[0] != 'r'){
        registers[target] = registers[a] + stoi(b);
    }
    else if (a[0] != 'r' && b[0] == 'r'){
        registers[target] = stoi(a) + registers[b];
    }
    else{
        registers[target] = stoi(a) + stoi(b);    
    } 
}

void Executor::sub(std::vector<std::string> op){

    auto itr = op.begin();
    std::string target = *itr;
    std::string a = *(++itr);
    std::string b = *(++itr);    
                
    if (a[0] == 'r' && b[0] == 'r'){
        registers[target] = registers[a] - registers[b];   
    }
    else if (a[0] == 'r' && b[0] != 'r'){
        registers[target] = registers[a] - stoi(b);
    }
    else if (a[0] != 'r' && b[0] == 'r'){
        registers[target] = stoi(a) - registers[b];
    }
    else{
        registers[target] = stoi(a) - stoi(b);    
    } 
}

void Executor::mul(std::vector<std::string> op){

    auto itr = op.begin();
    std::string target = *itr;
    std::string a = *(++itr);
    std::string b = *(++itr);    
                
    if (a[0] == 'r' && b[0] == 'r'){
        registers[target] = registers[a] * registers[b];   
    }
    else if (a[0] == 'r' && b[0] != 'r'){
        registers[target] = registers[a] * stoi(b);
    }
    else if (a[0] != 'r' && b[0] == 'r'){
        registers[target] = stoi(a) * registers[b];
    }
    else{
        registers[target] = stoi(a) * stoi(b);    
    } 
}

void Executor::div(std::vector<std::string> op){

    auto itr = op.begin();
    std::string target = *itr;
    std::string a = *(++itr);
    std::string b = *(++itr);    
                
    if (a[0] == 'r' && b[0] == 'r'){
        registers[target] = registers[a] / registers[b];   
    }
    else if (a[0] == 'r' && b[0] != 'r'){
        registers[target] = registers[a] / stoi(b);
    }
    else if (a[0] != 'r' && b[0] == 'r'){
        registers[target] = stoi(a) / registers[b];
    }
    else{
        registers[target] = stoi(a) / stoi(b);    
    } 

}

void Executor::mod(std::vector<std::string> op){

    auto itr = op.begin();    
    std::string target = *itr;
    std::string a = *(++itr);
    std::string b = *(++itr);    
                
    if (a[0] == 'r' && b[0] == 'r'){
        registers[target] = registers[a] % registers[b];   
    }
    else if (a[0] == 'r' && b[0] != 'r'){
        registers[target] = registers[a] % stoi(b);
    }
    else if (a[0] != 'r' && b[0] == 'r'){
        registers[target] = stoi(a) % registers[b];
    }
    else{
        registers[target] = stoi(a) % stoi(b);    
    }      
}

void Executor::inc(std::vector<std::string> op){

    auto itr = op.begin();
    registers[*itr] += 1;
}

void Executor::dec(std::vector<std::string> op){

    auto itr = op.begin();
    registers[*itr] -= 1;
}
    

