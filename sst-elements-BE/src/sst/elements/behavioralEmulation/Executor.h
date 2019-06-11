#ifndef EXECUTOR_H
#define EXECUTOR_H

#include "Instruction.h"
#include "Process.h"
#include "Routine.h"
#include "Simulation_Manager.h"
#include "signal.h"
#include <vector>
#include <queue>
#include <map>
#include <memory>
#include <sst/core/event.h>

namespace SST {
namespace BEComponent {

class simManager;

class Executor : public Process
{
public:
    explicit Executor(int g, int p, std::string program_file, std::shared_ptr<simManager> s, std::map<std::string, double> h_state) : Process() 
    {        
        gid = g;
        pid = p;
	filename = program_file;
	simulation_handler = s;
        state = h_state;
        Executor::buildProgram(filename);
        Executor::createImap();    
        //printf("Setting up the Executor for xeon E5 component\n");
        v_indep = false;
    	v_simul = false;  
    	instruction = program.begin(); 
        type = "Executor";
        eventId = 0;
    }

    Executor(const Executor&) = delete;
    Executor& operator=(const Executor&) = delete;
    ~Executor() = default;
	
    std::shared_ptr<simEvent> run();

    std::map<std::string, int> getRegisterSet() {
        return registers;
    }

    std::vector<Instruction>::iterator getCurrentInstruction() {
	return instruction-1;
    }

    std::tuple<std::vector<Instruction>, std::vector<Instruction>::iterator> getProgramState() {
        return std::tie(program, instruction);
    }

    int getEventId() {
        return ++eventId;
    }

    int gid;

private:    

    int pid, eventId;

    bool v_indep, v_simul;

    std::string filename;

    std::queue<std::shared_ptr<Process>> process_queue;	

    std::map<std::string, int> registers;

    /* floating point registers */
    std::map<std::string, float> fregisters; 

    std::map<std::string, std::vector<int>> registers_array;

    std::vector<Instruction> program;

    std::vector<Instruction>::iterator instruction;	

    std::shared_ptr<simManager> simulation_handler;

    typedef void (Executor::*FuncP)(std::vector<std::string>);    

    std::map<std::string, FuncP> inst_map;  	
    
    std::shared_ptr<simEvent> call(std::vector<std::string> op);

    std::shared_ptr<simEvent> comm(std::vector<std::string>::iterator op);

    std::shared_ptr<simEvent> prog(std::vector<std::string>::iterator op);

    std::shared_ptr<simEvent> barrier(std::vector<std::string>::iterator op);

    void indep(std::vector<std::string> op) {
        v_indep = true;
    }

    void simul(std::vector<std::string> op) {
        v_simul = true;
    }    

    std::shared_ptr<simEvent> begin(std::vector<std::string>::iterator op);

    void print(std::vector<std::string> op);

    void obtain(std::vector<std::string> op);

    void fobtain(std::vector<std::string> op);

    void assign(std::vector<std::string> op);

    void access(std::vector<std::string> op);

    void target(std::vector<std::string> op);

    void jumpeq(std::vector<std::string> op);

    void jumpnq(std::vector<std::string> op);

    void jumpgt(std::vector<std::string> op);

    void jumplt(std::vector<std::string> op);

    void jumpng(std::vector<std::string> op);

    void jumpnl(std::vector<std::string> op);

    void add(std::vector<std::string> op);

    void sub(std::vector<std::string> op);

    void mul(std::vector<std::string> op);

    void div(std::vector<std::string> op);

    void mod(std::vector<std::string> op);

    void inc(std::vector<std::string> op);

    void dec(std::vector<std::string> op);           
    
    void createImap();

    void buildProgram(std::string filename);    
    
};
}
}

#endif


