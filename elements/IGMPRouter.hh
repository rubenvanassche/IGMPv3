#ifndef CLICK_IGMPROUTER_HH
#define CLICK_IGMPROUTER_HH
#include <click/element.hh>
#include <click/timer.hh>
#include "IGMPRouterDB.hh"
#include "ProcessReport.hh"
#include "queryigmp.hh"

CLICK_DECLS

/*
=c

IGMPRouter(TODO)

=s

TODO: Summary

=d

TODO: Complete description

*/
class IGMPRouter : public Element {
	//TODO: Add private attributes

	public:
		IGMPRouter();
		~IGMPRouter();

		int configure(Vector<String>&, ErrorHandler*);

		void run_timer(Timer*);

		const char *class_name() const { return "IGMPRouter"; }
		const char *port_count() const { return "1/1"; }
		const char *processing() const { return PUSH; }

		int isINCLUDE(IPAddress client_address, IPAddress multicast_address);
		int isEXCLUDE(IPAddress client_address, IPAddress multicast_address);

		int toEX(IPAddress client_address, IPAddress multicast_address, Vector<IPAddress>& sources);
		int toIN(IPAddress client_address, IPAddress multicast_address, Vector<IPAddress>& sources);

		int block(IPAddress client_address, IPAddress multicast_address, Vector<IPAddress>& sources);
		int allow(IPAddress client_address, IPAddress multicast_address, Vector<IPAddress>& sources);

		int processReport(Packet *p);

		void push(int port, Packet *p);

		static String getDBHandler(Element *e, void * thunk);

		void add_handlers();
	private:
		IGMPRouterDB* db;
		QueryIGMPElement queryr;
		Timer timer;

		// Variables
		int robustness_variable = 2; // How many times resend
		int query_interval = 125; // Send each query_interval (seconds) an general query_interval
		int query_response_interval = 100; // for the MAX Response code, 100 is 10 seconds, must be < query_interval
		int group_memberschip_interval = robustness_variable*query_interval*query_response_interval; // Time when router decides there are no more members for a group or source from the network
		int last_member_query_interval = 10; // Max response for Group-specific queries
};

CLICK_ENDDECLS

#endif
