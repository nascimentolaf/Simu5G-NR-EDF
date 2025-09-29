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

#include "qos_data.h"
#include <algorithm> // for std::find_if

const qos_data::QoS5G get_qos_parameters(int qos_id)
{
    qos_data qos;

    // Compares each QoS5G struct's fiveQI with the provided qos_id
    // Returns true when it finds a match
    auto it = std::find_if(qos.QoS5GTable.begin(), qos.QoS5GTable.end(),
                           [qos_id](const qos_data::QoS5G &params)
                           {
                               return params.fiveQI == qos_id;
                           });

    // If found (it's not end()), returns the found QoS5G struct
    // If not found, returns empty QoS5G struct
    return (it != qos.QoS5GTable.end()) ? *it : qos_data::QoS5G{};
}
