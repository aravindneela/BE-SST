#ifndef CONTAINERS_H
#define CONTAINERS_H

#include <iostream>
#include <string>
#include <vector>
#include <map>
#include <utility>
#include <functional>
#include <random>

namespace SST{
namespace BEComponent{

/*
template <class T> class Procrastinator{
public:
	Procrastinator(T init, std::string t){
		initial = init;
		type = t; //attribute, input, output, random
	}
	
	~Procrastinator(){}
	Procrastinator<T>* operator+(double right);
    Procrastinator<T>* operator-(double right);
    Procrastinator<T>* operator*(double right);
    Procrastinator<T>* operator/(double right);
    Procrastinator<T>* operator^(double right);
    Procrastinator<T>* operator==(double right);
    Procrastinator<T>* operator!=(double right);
    Procrastinator<T>* operator<(double right);
    Procrastinator<T>* operator<=(double right);
    Procrastinator<T>* operator>(double right);
    Procrastinator<T>* operator>=(double right);  
    double operator()(std::map<std::string, double> d, std::vector<double> inp, std::vector<double> out);

private:
	T initial;	
	std::string type;
	std::vector<std::pair<std::function<double(double,double)>, double> > operations;
	
	double value(std::map<std::string, double> d, std::vector<int> inp, std::vector<double> out){
		if (type = "attribute")
			return d[initial];
		else if(type = "input")
			return inp[initial];
		else if(type = "output")
			return out[initial];
		else if(type = "random")
			return out[std::rand() % out.size()];	
	} 
	
	T value(){
		return initial;
	}
};
*/

//template<class T> static Procrastinator<T>* operator+ (double left, Procrastinator<T>& right);
//template<class T> static Procrastinator<T>* operator- (double left, Procrastinator<T>& right);
//template<class T> static Procrastinator<T>* operator* (double left, Procrastinator<T>& right);
//template<class T> static Procrastinator<T>* operator/ (double left, Procrastinator<T>& right);


class Procrastinator{
public:

    Procrastinator(){}

    Procrastinator(double init){
    	init = init;  
        type = "base";   
    }

    ~Procrastinator(){};

    std::string type;

    virtual double value(std::map<std::string, double> d, std::vector<float> inp, std::vector<double> out){
        printf("Inside base procrastinator value\n"); 
    	return init;
    } 

    Procrastinator* operate(std::string operation, double d_operand, Procrastinator* p_operand);
 
    double call(std::map<std::string, double> d, std::vector<float> inp, std::vector<double> out);  
    
protected:
    std::vector<std::tuple<std::function<double(double,double)>, double, Procrastinator*> > operations;
    
private:    
    double init;    
};

//static Procrastinator* operator+ (double left, Procrastinator& right);
//static Procrastinator* operator- (double left, Procrastinator& right);
//static Procrastinator* operator* (double left, Procrastinator& right);
//static Procrastinator* operator/ (double left, Procrastinator& right);

//Attribute Procrastinator
class attributeProcrastinator : public Procrastinator{
public:
    attributeProcrastinator(std::string init){
        initial = init;
        type = "attribute";
    }

    ~attributeProcrastinator(){};

    virtual double value(std::map<std::string, double> state, std::vector<float> inp, std::vector<double> out){      
        return state[initial];
    } 

private:
    std::string initial;
};

//Input Procrastinator
class inputProcrastinator : public Procrastinator{
public:
    inputProcrastinator(double init){
        initial = init;
        type = "input";
    }

    ~inputProcrastinator(){};

    virtual double value(std::map<std::string, double> state, std::vector<float> inp, std::vector<double> out){        
        if(initial < inp.size()) return double(inp[initial]);
        else return double(inp.back());
    }

private:
    double initial;
};

//Output Procrastinator
class outputProcrastinator : public Procrastinator{
public:
    outputProcrastinator(double init){
        initial = init;
        type = "output";
    }
    
    ~outputProcrastinator(){};

    virtual double value(std::map<std::string, double> state, std::vector<float> inp, std::vector<double> out){ 
        //printf("Inside output procrastinator value\n");       
        if(initial < out.size()) return out[initial];
        else return out.back();
    }

private:
    double initial;
};

//Random Procrastinator
class randomProcrastinator : public Procrastinator{
public:
    randomProcrastinator(double init){
        initial = init;  
        type = "random";  
    }

    ~randomProcrastinator(){};
    
    virtual double value(std::map<std::string, double> state, std::vector<float> inp, std::vector<double> out){              
        return out[std::rand() % out.size()];
    }

private:
    double initial;
};

}
}
#endif
