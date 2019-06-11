#include "Routine.h"
#include <iostream>

using namespace SST;
using namespace SST::BEComponent;

std::shared_ptr<simEvent> Routine::run()
{    
      //printf("routine run function\n");
    std::shared_ptr<simEvent> event;

    while (!sequence.empty()){

    	event_template = sequence.front();

        if (event_template->provision){  //find(event_template->provision)??

            if (event_template->type == "change"){
                if(event_template->val_type == "double"){
                    auto ev_temp = std::dynamic_pointer_cast<changeTemplate<double>> (event_template);
                    event = std::make_shared<changeEvent>(ev_temp->attribute, state, ev_temp->value);
                }
                else{
                    auto ev_temp = std::dynamic_pointer_cast<changeTemplate<Procrastinator*>> (event_template);  
                    event =  std::make_shared<changeEvent>(ev_temp->attribute, state, find(ev_temp->value));
                    } 
                               
            }
            else if (event_template->type == "timeout"){
         
                if(event_template->val_type == "double"){
                    auto ev_temp = std::dynamic_pointer_cast<timeoutTemplate<double>> (event_template);
                    event = std::make_shared<timeoutEvent>(ev_temp->value);
                }
                else{
                    auto ev_temp = std::dynamic_pointer_cast<timeoutTemplate<Procrastinator*>> (event_template);
                    event = std::make_shared<timeoutEvent>(find(ev_temp->value));
                }    
                  
            }
            else if (event_template->type == "condition"){

                if(event_template->val_type == "double"){
                    auto ev_temp = std::dynamic_pointer_cast<conditionTemplate<double>> (event_template);
                    event = std::make_shared<conditionEvent>(ev_temp->attribute, state, check[ev_temp->condition], ev_temp->value);
                }
                else{
                    auto ev_temp = std::dynamic_pointer_cast<conditionTemplate<Procrastinator*>> (event_template); 
                    event = std::make_shared<conditionEvent>(ev_temp->attribute, state, check[ev_temp->condition], find(ev_temp->value));
                }          
                
            }
            else if (event_template->type == "communicate"){

                auto ev_temp = std::dynamic_pointer_cast<communicateTemplate> (event_template);
                event = std::make_shared<commEvent>(ev_temp->pid, ev_temp->source, ev_temp->size, ev_temp->target, ev_temp->tag,  
                                                    ev_temp->tarlist, ev_temp->locations, ev_temp->next_hop, ev_temp->comm_type);
                event->type = "communicate";      
                
            }
            else if (event_template->type == "call"){

                auto ev_temp = std::dynamic_pointer_cast<callTemplate> (event_template);
                event = std::make_shared<callEvent>(ev_temp->source_gid, ev_temp->source_pid, ev_temp->target_gid, ev_temp->target_family, ev_temp->operation, ev_temp->inputs, 
                                                                 ev_temp->call_type);        
                
            }  
            else if (event_template->type == "acknowledge"){

                auto ev_temp = std::dynamic_pointer_cast<ackTemplate> (event_template);
                event = std::make_shared<ackEvent>(ev_temp->pid, ev_temp->locations, ev_temp->next_hop);       
                
            }  
            else if (event_template->type == "wait"){

                auto ev_temp = std::dynamic_pointer_cast<waitTemplate> (event_template);
                event = std::make_shared<waitEvent>();        
                
            }

            else if (event_template->type == "receive"){

                auto ev_temp = std::dynamic_pointer_cast<recvTemplate> (event_template);
                event = std::make_shared<recvEvent>();        
                
            } 

            else if (event_template->type == "receivewait"){

                auto ev_temp = std::dynamic_pointer_cast<recvWaitTemplate> (event_template);
                event = std::make_shared<recvWaitEvent>();        
                
            }      
                          
        }

        else{
            return NULL;        
        }
		
	sequence.pop();
	if (event)
          return event;

    } 
 
    return NULL; 	
}


double Routine::find(Procrastinator* v){

    double value = 0.0;

    if(v->type == "attribute"){
        attributeProcrastinator* p = dynamic_cast<attributeProcrastinator*>(v);
        value = p->call(state, input, output);
    }
    else if(v->type == "input"){
        inputProcrastinator* p = dynamic_cast<inputProcrastinator*>(v);
        value = p->call(state, input, output);
    }
    else if(v->type == "output"){
        outputProcrastinator* p = dynamic_cast<outputProcrastinator*>(v);
        value = p->call(state, input, output);
    }
    else if(v->type == "random"){
        randomProcrastinator* p = dynamic_cast<randomProcrastinator*>(v);
        value = p->call(state, input, output);
    }
    else value = v->call(state, input, output);
 
    return value;
	
}


//BOOST_CLASS_EXPORT(SST::Simulator::changeEvent)
//BOOST_CLASS_EXPORT(SST::Simulator::conditionEvent)
//BOOST_CLASS_EXPORT(SST::Simulator::timeoutEvent)
