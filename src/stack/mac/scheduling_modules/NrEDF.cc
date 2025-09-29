// Authors: Alaf Nascimento, Philippe Martins, Samuel Tardieu, and Laurent Pautet
// Contact: {firstname.lastname}@telecom-paris.fr
// Description: NR-EDF Scheduler
//

#include "stack/mac/LteMacBase.h"
#include "stack/mac/scheduling_modules/NrEDF.h"
#include "stack/mac/scheduler/LteSchedulerEnb.h"

namespace simu5g
{

    using namespace omnetpp;

    static inline int toResourceType(const std::string &s)
    {
        if (s == "DCGBR")
            return 0;
        if (s == "GBR")
            return 1;
        if (s == "NGBR")
            return 2;
        return -1;
    }

    // Function to compute priority
    double NrEDF::compute_tb_priority(const transport_block &tb)
    {
        auto params = get_qos_parameters(tb.mcp.qos_id);

        if (params.fiveQI == 0)
        {
            return -1; // Invalid QoS ID
        }

        double absolute_deadline = params.packet_delay_budget + tb.mcp.arrival_time;

        // Compute the priority based on the resource type and the 5QI parameters
        switch (toResourceType(params.resource_type))
        {
        case 0:
            return COMPUTE_PRIORITY_DCGBR(params, NOW.dbl(), tb);
        case 1:
            return COMPUTE_PRIORITY_GBR(params);
        case 2:
            return COMPUTE_PRIORITY_NGBR(params);
        default:
            return -1; // Invalid Resource Type
        }
    }

    double NrEDF::map_to_range(double min_value, double max_value, double value)
    {
        return min_value + (max_value - min_value) * value;
    }

    void NrEDF::queueing_by_resource_type(NrEDFScoreList &nrEdfQueue, double priority, MacCid cid, const transport_block &tb)
    {
        auto params = get_qos_parameters(tb.mcp.qos_id);

        // 3 bands: NGBR(0-32) < GBR(33-66) < DCGBR(+67)
        double mappedPriority = 0, max_value = 0, min_value = 0;
        switch (toResourceType(params.resource_type))
        {
        case 0: // DCGBR
            max_value = 100.0;
            min_value = 67.0;
            break;
        case 1: // GBR
            max_value = 66.0;
            min_value = 33.0;
            break;
        case 2: // NGBR
            max_value = 32.0;
            min_value = 0.0;
            break;
        default:
            break;
        }

        mappedPriority = map_to_range(min_value, max_value, priority);
        // std::cout << tb.mcp.qos_id << " | priority: " << priority << " (" << params.resource_type << ") mapped priority: " << mappedPriority << std::endl;
        // std::cout << "----------------------\n";
        nrEdfQueue.push(NrEdfScoreDesc(cid, mappedPriority));
    }

    int NrEDF::allocate_radio_resources(NrEDFScoreList &packets_queue)
    {
        while (!packets_queue.empty())
        {
            NrEdfScoreDesc current = packets_queue.top();

            // do not consider background traffic
            if (MacCidToNodeId(current.cid) >= BGUE_MIN_ID)
                continue;

            bool terminate = false, active = true, eligible = true;
            unsigned int granted = requestGrant(current.cid, std::numeric_limits<unsigned>::max(), terminate, active, eligible); // the scheduler computes how many bytes (or RBs) it can assign and extracts data accordingly —> this data forms the TB

            EV << "Granted " << granted << " bytes to cid " << current.cid << endl;
            // std::cout << NOW << " | Granted " << granted << " bytes to cid " << current.cid << std::endl;

            if (terminate)
            {
                EV << NOW << "NrEDF::execSchedule TERMINATE " << endl;
                break;
            }
            if (!active || !eligible)
            {
                packets_queue.pop();

                if (!eligible)
                    EV << NOW << "NrEDF::execSchedule NOT ELIGIBLE " << endl;
                else
                {
                    EV << NOW << "NrEDF::execSchedule NOT ACTIVE" << endl;
                    carrierActiveConnectionSet_.erase(current.cid);
                    activeConnectionTempSet_.erase(current.cid);

                    for (auto it = macPduMetaDataTempSet_.begin(); it != macPduMetaDataTempSet_.end();)
                    {
                        if (it->cid == current.cid)
                        {
                            it = macPduMetaDataTempSet_.erase(it);
                        }
                        else
                        {

                            ++it;
                        }
                    }
                }
            }
        }
        return 0;
    }

    void NrEDF::prepareSchedule()
    {
        EV << NOW << "NrEDF::execSchedule ############### gNodeB " << eNbScheduler_->mac_->getMacNodeId() << " ###############" << endl;

        // Create a working copy of the active set
        activeConnectionTempSet_ = *activeConnectionSet_;

        macPduMetaDataTempSet_ = *macPduMetaDataSet_;

        // Build the score list by cycling through the active connections.
        NrEDFScoreList nrEdfQueue;

        FiveQI fiveQi = 0;
        simtime_t Ai = 0;
        for (MacCid cid : carrierActiveConnectionSet_) // iterating over active connections, not over individual packets or transport blocks
        {
            MacNodeId nodeId = MacCidToNodeId(cid);
            LogicalCid lcid = MacCidToLcid(cid);
            OmnetId id = binder_->getOmnetId(nodeId);

            if (nodeId == NODEID_NONE || id == 0)
            {
                // node has left the simulation - erase corresponding CIDs
                activeConnectionSet_->erase(cid);
                activeConnectionTempSet_.erase(cid);
                carrierActiveConnectionSet_.erase(cid);

                for (auto it = macPduMetaDataTempSet_.begin(); it != macPduMetaDataTempSet_.end();)
                {
                    if (it->cid == cid)
                        it = macPduMetaDataTempSet_.erase(it);

                    else
                        ++it;
                }
                continue;
            }

            for (auto it = macPduMetaDataTempSet_.rbegin(); it != macPduMetaDataTempSet_.rend(); ++it)
            {
                if (it->cid == cid)
                {
                    fiveQi = it->fiveQi;
                    Ai = it->arrivalTime;
                    break; // we wanted the last one
                }
            }

            rlc_pdu rlcPDU = {uint8_t(fiveQi), Ai.dbl()};         // RLC PDU metadata
            double wctt = 0;                                      // TODO
            transport_block tb = {rlcPDU, simTime().dbl(), wctt}; // Initialize transport block with the last RLC PDU metadata
            double priority = compute_tb_priority(tb);
            queueing_by_resource_type(nrEdfQueue, priority, cid, tb);
        }

        allocate_radio_resources(nrEdfQueue);
    }

    void NrEDF::commitSchedule()
    {
        *activeConnectionSet_ = activeConnectionTempSet_;
        *macPduMetaDataSet_ = macPduMetaDataTempSet_;
    }

} // namespace
