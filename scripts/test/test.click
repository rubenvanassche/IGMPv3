igmpDB::IGMPClientDB;
igmpClient::IGMPClient(DB igmpDB);
// igmpClientResponder::IGMPClientResponder(DB igmpDB);
// igmpClassifier::IGMPClassifier(DB igmpDB);

routerDB::IGMPRouterDB;
//router::IGMPRouter(DB routerDB);
routerClassifier::IGMPRouterClassifier(DB routerDB);

igmpClient-> IPEncap(4, 192.168.1.1, 224.0.0.22, TTL 1, PROTO 2)
           -> routerClassifier;

routerClassifier[0] -> Discard;
routerClassifier[1] -> Discard;
routerClassifier[2] -> Discard;
