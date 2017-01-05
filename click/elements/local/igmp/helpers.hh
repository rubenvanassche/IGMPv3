#ifndef CLICK_HELPERS_HH
#define CLICK_HELPERS_HH
#include <click/element.hh>
#include <click/vector.cc>

#if EXPLICIT_TEMPLATE_INSTANCES
template class Vector<SomeThing>;
#endif

CLICK_DECLS

class ConfigParse{
public:
    Vector<IPAddress> sources(const String &conf, ErrorHandler * errh);
    IPAddress multicastAddress(const String &conf, ErrorHandler * errh);
};

CLICK_ENDDECLS

#endif
