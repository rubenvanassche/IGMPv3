#ifndef CLICK_DECODEREPORTIGMP_HH
#define CLICK_DECODEREPORTIGMP_HH
#include <click/element.hh>
#include <click/vector.cc>
#include "packets.hh"

#if EXPLICIT_TEMPLATE_INSTANCES
template class Vector<SomeThing>;
#endif
CLICK_DECLS

class DecodeReportIGMP : public Element {
	public:
		DecodeReportIGMP();
		~DecodeReportIGMP();

		const char *class_name() const	{ return "DecodeReportIGMP"; }
		const char *port_count() const	{ return "1/0"; }
		const char *processing() const	{ return PUSH; }
		int configure(Vector<String>&, ErrorHandler*);

		bool verifyChecksum(Packet* p);

		void push(int, Packet *);
	private:
		uint32_t maxSize;
};

CLICK_ENDDECLS
#endif
