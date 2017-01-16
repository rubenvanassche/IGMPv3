// Router with three interfaces
// The input/output configuration is as follows:
//
// Input:
//	[0]: packets received on the 192.168.1.0/24 network
//	[1]: packets received on the 192.168.2.0/24 network
//	[2]: packets received on the 192.168.3.0/24 network
//
// Output:
//	[0]: packets sent to the 192.168.1.0/24 network
//	[1]: packets sent to the 192.168.2.0/24 network
//	[2]: packets sent to the 192.168.3.0/24 network
//  [3]: packets destined for the router itself

elementclass Router {
	$server_address, $client1_address, $client2_address |

	// IGMP elements
	igmpDB::IGMPRouterDB();
	igmpRouter::IGMPRouter(DB igmpDB);
	igmpClassifier::IGMPRouterClassifier(DB igmpDB);

	igmpPacketCopy::Tee(2);
	igmpPacketCopyQuery::Tee(2);

	client1_igmpRouterSender::IGMPRouterSender(DB igmpDB, ADDR $client1_address:ipnet);
	client2_igmpRouterSender::IGMPRouterSender(DB igmpDB, ADDR $client2_address:ipnet);

	igmpClassifier[2] -> igmpRouter;
	igmpClassifier[1] -> igmpPacketCopy;

	igmpPacketCopy[0]->client1_igmpRouterSender;
	igmpPacketCopy[1]->client2_igmpRouterSender;

	client1_igmpRouterSender ->  EtherEncap(0x0800, $client1_address:ether, FF:FF:FF:FF:FF:FF) -> [1]output;
	client2_igmpRouterSender -> EtherEncap(0x0800, $client2_address:ether, FF:FF:FF:FF:FF:FF)  	-> [2]output;

	// Send queries over the interfaces
	igmpRouter[0] -> igmpPacketCopyQuery;

	igmpPacketCopyQuery[0]
		-> SetIPSrc(ADDR $client1_address:ip)
		-> SetIPChecksum
		-> EtherEncap(0x0800, $client1_address:ether, FF:FF:FF:FF:FF:FF)
		-> ToDump("qi.dump")
		-> [1]output

	igmpPacketCopyQuery[1]
		-> SetIPSrc(ADDR $client2_address:ip)
		-> SetIPChecksum
		->  EtherEncap(0x0800, $client2_address:ether, FF:FF:FF:FF:FF:FF)
		-> [2]output


	// Shared IP input path and routing table
	ip :: Strip(14)
		-> CheckIPHeader
		-> igmpClassifier
		-> rt :: StaticIPLookup(
					$server_address:ip/32 0,
					$client1_address:ip/32 0,
					$client2_address:ip/32 0,
					$server_address:ipnet 1,
					$client1_address:ipnet 2,
					$client2_address:ipnet 3);

	// ARP responses are copied to each ARPQuerier and the host.
	arpt :: Tee (3);

	// Input and output paths for interface 0
	input
		-> HostEtherFilter($server_address)
		-> server_class :: Classifier(12/0806 20/0001, 12/0806 20/0002, -)
		-> ARPResponder($server_address)
		-> output;

	server_arpq :: ARPQuerier($server_address)
		-> output;

	server_class[1]
		-> arpt
		-> [1]server_arpq;

	server_class[2]
		-> Paint(1)
		-> ip;

	// Input and output paths for interface 1
	input[1]
		-> HostEtherFilter($client1_address)
		-> client1_class :: Classifier(12/0806 20/0001, 12/0806 20/0002, -)
		-> ARPResponder($client1_address)
		-> [1]output;

	client1_arpq :: ARPQuerier($client1_address)
		-> [1]output;

	client1_class[1]
		-> arpt[1]
		-> [1]client1_arpq;

	client1_class[2]
		-> Paint(2)
		-> ip;

	// Input and output paths for interface 2
	input[2]
		-> HostEtherFilter($client2_address)
		-> client2_class :: Classifier(12/0806 20/0001, 12/0806 20/0002, -)
		-> ARPResponder($client2_address)
		-> [2]output;

	client2_arpq :: ARPQuerier($client2_address)
		-> [2]output;

	client2_class[1]
		-> arpt[2]
		-> [1]client2_arpq;

	client2_class[2]
		-> Paint(3)
		-> ip;

	// Local delivery
	rt[0]
		-> [3]output

	// Forwarding paths per interface
	rt[1]
		-> DropBroadcasts
		-> server_paint :: PaintTee(1)
		-> server_ipgw :: IPGWOptions($server_address)
		-> FixIPSrc($server_address)
		-> server_ttl :: DecIPTTL
		-> server_frag :: IPFragmenter(1500)
		-> server_arpq;

	server_paint[1]
		-> ICMPError($server_address, redirect, host)
		-> rt;

	server_ipgw[1]
		-> ICMPError($server_address, parameterproblem)
		-> rt;

	server_ttl[1]
		-> ICMPError($server_address, timeexceeded)
		-> rt;

	server_frag[1]
		-> ICMPError($server_address, unreachable, needfrag)
		-> rt;


	rt[2]
		-> DropBroadcasts
		-> client1_paint :: PaintTee(2)
		-> client1_ipgw :: IPGWOptions($client1_address)
		-> FixIPSrc($client1_address)
		-> client1_ttl :: DecIPTTL
		-> client1_frag :: IPFragmenter(1500)
		-> client1_arpq;

	client1_paint[1]
		-> ICMPError($client1_address, redirect, host)
		-> rt;

	client1_ipgw[1]
		-> ICMPError($client1_address, parameterproblem)
		-> rt;

	client1_ttl[1]
		-> ICMPError($client1_address, timeexceeded)
		-> rt;

	client1_frag[1]
		-> ICMPError($client1_address, unreachable, needfrag)
		-> rt;


	rt[3]
		-> DropBroadcasts
		-> client2_paint :: PaintTee(2)
		-> client2_ipgw :: IPGWOptions($client2_address)
		-> FixIPSrc($client2_address)
		-> client2_ttl :: DecIPTTL
		-> client2_frag :: IPFragmenter(1500)
		-> client2_arpq;

	client2_paint[1]
		-> ICMPError($client2_address, redirect, host)
		-> rt;

	client2_ipgw[1]
		-> ICMPError($client2_address, parameterproblem)
		-> rt;

	client2_ttl[1]
		-> ICMPError($client2_address, timeexceeded)
		-> rt;

	client2_frag[1]
		-> ICMPError($client2_address, unreachable, needfrag)
		-> rt;
}
