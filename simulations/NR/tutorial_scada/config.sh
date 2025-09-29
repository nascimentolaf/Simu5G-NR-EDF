cd ~/omnetpp-6.1/
source setenv

cd ~/omnetpp-6.1/workspace/inet4.5
source setenv
make makefiles
make -j12

cd ~/omnetpp-6.1/workspace/simu5g
make makefiles
make -j12
. setenv

echo 'go to simulations/NR/tutorial_scada and run /run -u Cmdenv -f omnetpp.ini -c Single-UE -r 0..9999'
