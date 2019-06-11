#ifndef EVENT_TEMPLATE_H
#define EVENT_TEMPLATE_H

#include<sst/core/event.h>
#include <queue>

namespace SST {
namespace BEComponent {

 
class eventTemplate {	
public:

    eventTemplate(){}
    virtual ~eventTemplate(){}

    std::string type;
    std::string attribute;
    bool provision;
    std::string val_type;

};

template <class Dtype>
class timeoutTemplate : public eventTemplate {	
public:

    timeoutTemplate(Dtype v, std::string v_type, bool prov=true) {
        value = v;    
        provision = prov;
        type = "timeout";
        val_type = v_type;
    }
    virtual ~timeoutTemplate(){}

    Dtype value;

};

template <class Dtype>
class changeTemplate : public eventTemplate {	
public:

    changeTemplate(std::string attr, Dtype v, std::string v_type, bool prov=true) {
        attribute = attr;
        value = v;
        provision = prov;
        type = "change";
        val_type = v_type;
    }
    virtual ~changeTemplate(){}

    Dtype value;

};

template <class Dtype>
class conditionTemplate : public eventTemplate {	
public:

    conditionTemplate(std::string attr, std::string c, Dtype v, std::string v_type, bool prov=true) {
        attribute = attr;
        value = v;
        condition = c;
        provision = prov;
        type = "condition";
        val_type = v_type;
    }
    virtual ~conditionTemplate(){}

    std::string condition;
    Dtype value;

};

class communicateTemplate : public eventTemplate {	
public:

    communicateTemplate(int pi, int so, int si, int tar, int tg, std::vector<int> tlist, std::vector<int> locs, int nh, std::string ct, bool prov=true) {
        pid = pi;
        source = so;
        size = si;
        target = tar;
        tag = tg;
        tarlist = tlist;
        locations = locs;
        next_hop = nh;
        comm_type = ct;
        type = "communicate";
        provision = prov;
    }
    virtual ~communicateTemplate(){}

    int pid, source, size, target, tag, next_hop;
    std::string comm_type;
    std::vector<int> locations, tarlist;

};

class callTemplate : public eventTemplate {	
public:

    callTemplate(int sg, int spid, int trg, std::string tar, std::string ops, std::vector<float> ips, std::string ct, bool prov=true) {
        source_gid = sg;
        source_pid = spid;
        target_gid = trg;
        target_family = tar;
        operation = ops;
        inputs = ips;
        call_type = ct;
        type = "call";
        provision = prov;
    }
    virtual ~callTemplate(){}

    int source_gid, source_pid, target_gid;
    std::string target_family, operation, call_type;
    std::vector<float> inputs;

};

class recvTemplate : public eventTemplate {	
public:

    recvTemplate(bool prov=true) {
        type = "receive";
        provision = prov;
    }
    virtual ~recvTemplate(){}

};

class recvWaitTemplate : public eventTemplate {	
public:

    recvWaitTemplate(bool prov=true) {
        type = "receivewait";
        provision = prov;
    }
    virtual ~recvWaitTemplate(){}

};

class ackTemplate : public eventTemplate {	
public:

    ackTemplate(int pi, std::vector<int> loc, int nh, bool prov=true) {
        locations = loc;
        pid = pi;
        next_hop = nh;
        type = "acknowledge";
        provision = prov;
    }
    virtual ~ackTemplate(){}

    std::vector<int> locations;
    int pid, next_hop;

};

class waitTemplate : public eventTemplate {	
public:

    waitTemplate(bool prov=true) {
        type = "wait";
        provision = prov;
    }
    virtual ~waitTemplate(){}

};


}
}

#endif
 
