#include "helpers.hh"

CLICK_DECLS

Vector<IPAddress> ConfigParse::sources(const String &conf, ErrorHandler * errh){
	String sources_string;
	String source_address;
    String multicast_address;
	Vector<IPAddress> sources;

	// Parse
	if(Args(errh)
	.push_back_args(conf)
	.read_m("M", multicast_address)
	.read_m("S", sources_string)
	.complete() < 0){
		click_chatter("Failed reading include sources");

		return sources;
	}

	// Seperate the sources_string
	for(int i = 0;i < sources_string.length(); i++){
		if(sources_string.at(i) == '+'){
			IPAddress addr(source_address);
			source_address = "";

			sources.push_back(addr);
		}else{
			source_address += sources_string.at(i);
		}

		if(i == sources_string.length() - 1){
			IPAddress addr(source_address);
			sources.push_back(addr);
		}
	}

    return sources;
}

IPAddress ConfigParse::multicastAddress(const String &conf, ErrorHandler * errh){
    String sources_string;
    String multicast_address;

	// Parse
	if(Args(errh)
	.push_back_args(conf)
	.read_m("M", multicast_address)
	.read_m("S", sources_string)
	.complete() < 0){
		click_chatter("Failed reading include sources");

		return IPAddress("0.0.0.0");
	}

    return IPAddress(multicast_address);


}

CLICK_ENDDECLS
