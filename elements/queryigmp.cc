#ifndef CLICK_QUERYIGMPELEMENT_HH
#define CLICK_QUERYIGMPELEMENT_HH
#include <click/element.hh>

CLICK_DECLS
class QueryIGMPElement : public Element{
public:
    queryIGMPElement();
    ~queryIGMPElement();
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
