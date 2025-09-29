//
//                  Simu5G-NR-EDF (Extension of Simu5G)
//
// Original Authors: Giovanni Nardini, Giovanni Stea, Antonio Virdis (University of Pisa)
// Extension Authors: Alaf Nascimento, Philippe Martins, Samuel Tardieu, Laurent Pautet (Institut Polytechnique de Paris)
//
// This file is part of a software released under the license included in file
// "license.pdf". Please read LICENSE and README files before using it.
// The above files and the present reference are part of the software itself,
// and cannot be removed from it.
//
#ifndef _CBRRECEIVER_H_
#define _CBRRECEIVER_H_

#include <string.h>
#include <omnetpp.h>

#include <inet/common/INETDefs.h>
#include <inet/transportlayer/contract/udp/UdpSocket.h>
#include <inet/networklayer/common/L3AddressResolver.h>

#include "CbrPacket_m.h"

namespace simu5g
{

  using namespace omnetpp;

  class CbrReceiver : public cSimpleModule
  {
    inet::UdpSocket socket;

    int numReceived_;
    int totFrames_;
    int recvBytes_;

    bool mInit_;

    static simsignal_t cbrFrameLossSignal_;
    static simsignal_t cbrFrameDelaySignal_;
    static simsignal_t cbrJitterSignal_;
    static simsignal_t cbrReceivedThroughputSignal_;
    static simsignal_t cbrReceivedBytesSignal_;
    static simsignal_t cbrRcvdPktSignal_;

    // alaf
    static simsignal_t cbrMissedPktDeadlineSignal_;
    static simsignal_t cbrPktCounterSignal_;  
    
    //alaf
    int counter_total = 0;
    int counter_missed = 0;

  protected:
    int numInitStages() const override { return inet::NUM_INIT_STAGES; }
    void initialize(int stage) override;
    void handleMessage(cMessage *msg) override;
    void finish() override;
  };

} // namespace

#endif
