#ifndef CLICK_QUERYIGMPELEMENT_HH
#define CLICK_QUERYIGMPELEMENT_HH
#include <click/element.hh>
#include <click/vector.cc>
#include "packets.hh"

#if EXPLICIT_TEMPLATE_INSTANCES
template class Vector<SomeThing>;
#endif

CLICK_DECLS
typedef Vector<in_addr> sourcesVector;

class QueryIGMPElement : public Element{
public:
	QueryIGMPElement();
    ~QueryIGMPElement();

    const char *class_name() const { return "QueryIGMPElement"; }
    const char *port_count() const { return "1/1"; }
    const char *processing() const { return PUSH; }
    int configure(Vector<String>&, ErrorHandler*);
    void push(int, Packet*);
private:
    uint32_t maxSize;
};
CLICK_ENDDECLS

#endif
