#ifndef CLICK_IGMPClient_HH
#define CLICK_IGMPClient_HH
#include <click/element.hh>
#include <click/vector.cc>
#include <click/args.hh>
#include <assert.h>
#include "packets.hh"
#include "reportigmp.hh"
#include "IGMPClientDB.hh"
#include "ProcessQuery.hh"

#if EXPLICIT_TEMPLATE_INSTANCES
template class Vector<SomeThing>;
#endif

CLICK_DECLS
class IGMPClient : public Element {
	public:
		IGMPClient();
		~IGMPClient();

		int configure(Vector<String>&, ErrorHandler*);

		const char *class_name() const { return "IGMPClient"; }
		const char *port_count() const { return "1/1"; }
		const char *processing() const { return PUSH; }

		void push(int port, Packet *p);

		void proccessQuery(Packet *p);

		void includeWithExclude(IPAddress multicast_address, Vector<IPAddress> sources);
		void includeWithInclude(IPAddress multicast_address, Vector<IPAddress> sources);
		void excludeWithExclude(IPAddress multicast_address, Vector<IPAddress> sources);
		void excludeWithInclude(IPAddress multicast_address, Vector<IPAddress> sources);

		void generalQuery(ProcessQuery &pq);
		void groupQuery(ProcessQuery &pq);
		void groupAndSourceQuery(ProcessQuery &pq);

		// Handlers
		// Use: M {IPAdresss}, S {IPADRESS}
		// Use: M {IPAdresss}, S {IPADRESS}+{IPADRESS}
		static int includeSourcesHandler(const String &conf, Element *e, void * thunk, ErrorHandler * errh);
		static int excludeSourcesHandler(const String &conf, Element *e, void * thunk, ErrorHandler * errh);

		static String getDBHandler(Element *e, void * thunk);

		void add_handlers();
	private:
		ReportIGMPElement reporter;
		IGMPClientDB* db;
		IPAddress ipAddress;


		// Send report timer
		struct SendReportTimerData{
			IGMPClient* me;
			Packet* report;
		};

		static void handleSendReportTimer(Timer*, void *);

		// Variables
		int unsolicited_report_interval = 1; // Send reports after x seconds random from [0, unsolicited_report_interval]
		int robustness_variable = 2;
};

class ConfigParse{
public:
    Vector<IPAddress> sources(const String &conf, ErrorHandler * errh);
    IPAddress multicastAddress(const String &conf, ErrorHandler * errh);
};

// Vector Operations
Vector<IPAddress> vectorsUnion(Vector<IPAddress> a, Vector<IPAddress> b);
Vector<IPAddress> vectorsIntersection(Vector<IPAddress> a, Vector<IPAddress> b);
Vector<IPAddress> vectorsMinus(Vector<IPAddress> a, Vector<IPAddress> b);
Vector<IPAddress> vectorsUnique(Vector<IPAddress> a);

CLICK_ENDDECLS

#endif
