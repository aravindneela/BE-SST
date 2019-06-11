#ifndef INSTRUCTION_H
#define	INSTRUCTION_H

#include <string>
#include <vector>
#include <sstream>
#include <iostream>

namespace SST {
namespace BEComponent {

class Instruction{
public:
	std::string kind;
	std::vector<std::string> operands;
	int line;

	Instruction(std::string l, std::string k, std::string o){
		kind = k;		
		line = stoi(l);
		operands = decode(o);	        		
	}	

private:

        std::vector<std::string> decode(std::string operand)
        {	
          std::stringstream s(operand);
          std::vector<std::string> operandvector;
          while (!s.eof()){
            std::string tmp;
            s >> tmp;
            operandvector.push_back(tmp);            
          }  
          return operandvector;              
	}
};
}
}
#endif
