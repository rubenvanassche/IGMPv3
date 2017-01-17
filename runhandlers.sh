#!/bin/sh
(
echo "write client21/igmpClient.exclude_sources M 239.0.0.0, S 1.1.1.1"
sleep 2
echo "write client22/igmpClient.exclude_sources M 239.0.0.0, S 1.1.1.1"
sleep 2
echo "write client31/igmpClient.exclude_sources M 239.0.0.0, S 1.1.1.1"
sleep 2
echo "write client32/igmpClient.exclude_sources M 239.0.0.0, S 1.1.1.1"
sleep 2
echo "write client21/igmpClient.include_sources M 239.0.0.0, S 1.1.1.1"
sleep 2
echo "write client22/igmpClient.include_sources M 239.0.0.0, S 1.1.1.1"
sleep 2
echo "write client31/igmpClient.include_sources M 239.0.0.0, S 1.1.1.1"
sleep 2
echo "write client32/igmpClient.include_sources M 239.0.0.0, S 1.1.1.1"
sleep 2
echo "quit"
) | telnet 127.0.0.1 1981
