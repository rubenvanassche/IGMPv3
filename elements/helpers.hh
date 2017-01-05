#ifndef CLICK_PACKETS_HH
#define CLICK_PACKETS_HH
#include <click/element.hh>
#include <click/String.cc>
#include <click/vector.cc>

#if EXPLICIT_TEMPLATE_INSTANCES
template class Vector<SomeThing>;
#endif

Vector<IPAdresss> parseSourcesAdressesFromConfig(const String &conf, ErrorHandler * errh){

}

IPAddress parseSourcesFromConfig(const String &conf, ErrorHandler * errh){

}

#endif
