// Authors: Alaf Nascimento, Philippe Martins, Samuel Tardieu, and Laurent Pautet
// Contact: {firstname.lastname}@telecom-paris.fr
// Description: NR EDF Scheduler
//

// #define NREDF_DEBUG // Uncomment to enable debug output

#ifndef _NR_EDF_SCHEDULER_GNB_H_
#define _NR_EDF_SCHEDULER_GNB_H_

#include "stack/mac/scheduler/LteScheduler.h"
#include "common/qos_data.h"
#include <queue>
#include <iostream>

#define NORMALIZE(max, min, x) \
  ((x - min) / (max - min))

#define REV_NORMALIZE(max, min, x) \
  ((max - x) / (max - min))

#define COMPUTE_PRIORITY_DCGBR(params, t, tb) \
  (t/(tb.mcp.arrival_time + params.packet_delay_budget)); // Di = Ai+PDF, P = t/Di, packet drop when P = 1

#define COMPUTE_PRIORITY_GBR(params) \
  (REV_NORMALIZE(MAX_GBR_LOG_PER, MIN_GBR_LOG_PER, params.packet_error_rate_exp));

#define COMPUTE_PRIORITY_NGBR(params) \
  (REV_NORMALIZE(MAX_NGBR_DLP, MIN_NGBR_DLP, params.default_priority_level));

namespace simu5g
{

  class NrEDF : public LteScheduler
  {

  protected:
    struct NrEdfScoreDesc // based on the SortedDesc structure
    {
      MacCid cid;      // Connection ID
      double priority; // Priority score

      NrEdfScoreDesc(MacCid c, double p) : cid(c), priority(p) {}
      bool operator<(const NrEdfScoreDesc &other) const
      {
        return priority < other.priority; // descending order of priority (highest priority comes first), use top() afterwards
      }
    };

    // RLC PDU Metadata
    struct rlc_pdu
    {
      int qos_id;          // 5QI
      double arrival_time; // Ai
    };

    // Transport Block
    struct transport_block
    {
      rlc_pdu mcp;                 // Most Critical Packet
      double scheduling_time = -1; // Moment when the TB is sent to a HARQ
      double wctt = -1;            // Worst Case Transmission Time
    };

    typedef std::priority_queue<NrEdfScoreDesc> NrEDFScoreList;

    // Resource Type Based Priority
    double compute_tb_priority(const transport_block &tb);
    void queueing_by_resource_type(NrEDFScoreList &nrEdfQueue, double priority, MacCid cid, const transport_block &tb);
    int allocate_radio_resources(NrEDFScoreList &packets_queue);
    double map_to_range(double min_value, double max_value, double value);

  public:
    NrEDF(Binder *binder) : LteScheduler(binder)
    {
    }
    void prepareSchedule() override;
    void commitSchedule() override;
  };

} // namespace

#endif // _NR_EDF_SCHEDULER_GNB_H_
