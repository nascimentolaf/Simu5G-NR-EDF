NR-EDF Simu5G Extension
=======================

This repository provides an NR-EDF (New Radio – Earliest Deadline First) extension for Simu5G OMNeT++ library. It introduces new scheduling and analysis features tailored for 5G simulations with real-time traffic constraints.
Refer to [README_SIMU5G.md](./README_SIMU5G.md) for the original Simu5G 1.3.0 documentation.

Core contributors
-----------------
- Alaf Nascimento (alaf.nascimento@telecom-paris.fr)
- Philippe Martins (philippe.martins@telecom-paris.fr)
- Samuel Tardieu (samuel.tardieu@telecom-paris.fr)
- Laurent Pautet (laurent.pautet@telecom-paris.fr)

Dependencies
------------
This version requires:
- OMNeT++ 6.1.0
- INET 4.5.4

Simu5G Features
---------------
- NR-EDF Scheduler
- Missed Deadlines Counter for periodic and sporadic traffic
- Python scripts for:
  - Converting `.sca` files into `.json` files
  - Analysing `.sca` and `.json` files (JSON is more storage-efficient)

