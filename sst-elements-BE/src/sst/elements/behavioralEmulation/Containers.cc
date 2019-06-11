#include "Containers.h"

using namespace SST;
using namespace SST::BEComponent;


Procrastinator* Procrastinator::operate(std::string operation, double d_operand, Procrastinator* p_operand)
{

    if(operation == "+"){
        Procrastinator* tmp = this;   
        tmp->operations.push_back(std::make_tuple(std::plus<double>(), d_operand, p_operand));
        return tmp;
    }

    else if(operation == "-"){
        Procrastinator* tmp = this;   
        tmp->operations.push_back(std::make_tuple(std::minus<double>(), d_operand, p_operand));
        return tmp;
    }

    else if(operation == "*"){
        Procrastinator* tmp = this;   
        tmp->operations.push_back(std::make_tuple(std::multiplies<double>(), d_operand, p_operand));
        return tmp;
    }

    else if(operation == "/"){
        Procrastinator* tmp = this;   
        tmp->operations.push_back(std::make_tuple(std::divides<double>(), d_operand, p_operand));
        return tmp;
    }

    else if(operation == "^"){
        Procrastinator* tmp = this;   
        tmp->operations.push_back(std::make_tuple(std::plus<double>(), d_operand, p_operand));
        return tmp;
    }

    else if(operation == "=="){
        Procrastinator* tmp = this;   
        tmp->operations.push_back(std::make_tuple(std::equal_to<double>(), d_operand, p_operand));
        return tmp;
    }

    else if(operation == "!="){
        Procrastinator* tmp = this;   
        tmp->operations.push_back(std::make_tuple(std::not_equal_to<double>(), d_operand, p_operand));
        return tmp;
    }

    else if(operation == "<"){
        Procrastinator* tmp = this;   
        tmp->operations.push_back(std::make_tuple(std::less<double>(), d_operand, p_operand));
        return tmp;
    }

    else if(operation == "<="){
        Procrastinator* tmp = this;   
        tmp->operations.push_back(std::make_tuple(std::less_equal<double>(), d_operand, p_operand));
        return tmp;
    }

    else if(operation == ">"){
        Procrastinator* tmp = this;   
        tmp->operations.push_back(std::make_tuple(std::greater<double>(), d_operand, p_operand));
        return tmp;
    }

    else if(operation == ">="){
        Procrastinator* tmp = this;   
        tmp->operations.push_back(std::make_tuple(std::greater_equal<double>(), d_operand, p_operand));
        return tmp;
    }

    else{
        Procrastinator* tmp = this;
        std::cout<<"The operator "<<operation<<" is not supported. Ignoring the operation. \n";
        return tmp;
    }


}


//check: might need to be changed a bit
double Procrastinator::call(std::map<std::string, double> state, std::vector<float> inp, std::vector<double> out)
{

    double rhs;
    double lhs = value(state, inp, out);   

    for (auto itr = operations.begin(); itr != operations.end(); ++itr)
    {
        if (std::get<2>(*itr) != NULL)
	    rhs = std::get<2>(*itr)->call(state, inp, out);		
	else
	    rhs = std::get<1>(*itr);
	
	lhs = std::get<0>(*itr)(lhs, rhs);
    }

    return lhs;

}

/*
template <class T> Procrastinator<T>* Procrastinator<T>::operator+(double right){
    Procrastinator* tmp = new Procrastinator(this);   
    tmp->operations.push_back(std::make_pair(std::plus<double>(), right));
    return tmp;
}

template <class T> Procrastinator<T>* Procrastinator<T>::operator-(double right){

}

template <class T> Procrastinator<T>* Procrastinator<T>::operator*(double right){

}

template <class T> Procrastinator<T>* Procrastinator<T>::operator/(double right){

}

template <class T> Procrastinator<T>* Procrastinator<T>::operator^(double right){

}

template <class T> Procrastinator<T>* Procrastinator<T>::operator==(double right){

}

template <class T> Procrastinator<T>* Procrastinator<T>::operator!=(double right){

}

template <class T> Procrastinator<T>* Procrastinator<T>::operator<(double right){

}

template <class T> Procrastinator<T>* Procrastinator<T>::operator<=(double right){

}

template <class T> Procrastinator<T>* Procrastinator<T>::operator>(double right){

}

template <class T> Procrastinator<T>* Procrastinator<T>::operator>=(double right){

}

template <class T> double Procrastinator<T>::operator()(std::map<std::string, double> d, std::vector<double> inp, std::vector<double> out){
	double rhs;
	double lhs = value(d, inp, out);
	std::vector<std::pair<std::function<double(double,double)>, double> > operations;
	
	for (auto itr = operations.begin(); itr != operations.end(); ++itr){
		if (typeid(itr->second).name() == "Procrastinator")
			rhs = value(d, inp, out);		
		else
			rhs = itr->second;
		
		lhs = itr->first(lhs, rhs);
	}
	return lhs;
}
*/

