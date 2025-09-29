# NR-EDF Simu5G Extension

This repository provides an NR-EDF (New Radio – Earliest Deadline First) extension for Simu5G OMNeT++ library. It introduces new scheduling and analysis features tailored for 5G simulations with real-time traffic constraints.
Refer to [README_SIMU5G.md](./README_SIMU5G.md) for the original Simu5G 1.3.0 documentation.

## Core contributors
- Alaf Nascimento (alaf.nascimento@telecom-paris.fr)
- Philippe Martins (philippe.martins@telecom-paris.fr)
- Samuel Tardieu (samuel.tardieu@telecom-paris.fr)
- Laurent Pautet (laurent.pautet@telecom-paris.fr)

## Dependencies
This version requires:
- OMNeT++ 6.1.0
- INET 4.5.4

## Simu5G Features
- NR-EDF Scheduler
- Missed Deadlines Counter for periodic and sporadic traffic
- Python scripts for:
  - Converting `.sca` files into `.json` files
  - Analysing `.sca` and `.json` files (JSON is more storage-efficient)


## How to use this extension
We assume you have your folders organized as follows:

```bash
~/omnetpp-6.1/
├── workspace/
│   ├── inet4.5/
│   └── simu5g/
```

Navigate to the simulation directory and configure the environment:
```bash
# Go to the SCADA tutorial simulations folder
$ cd simulations/NR/tutorial_scada

# Set up the configuration for the simulations
$ ./config.sh
```

### 1 - Run the simulation scripts

Run the simulations with different schedulers. The output is redirected to /dev/null to keep the terminal clean:

```bash
# Run with NR-EDF Scheduler
$ opp_runall -j"$(nproc)" ./run -u Cmdenv -f omnetpp_dcgbr_v7.ini -c EDF_Scheduler -r 0..99999 > /dev/null 2>&1

# Run with PF Scheduler
$ opp_runall -j"$(nproc)" ./run -u Cmdenv -f omnetpp_dcgbr_v7.ini -c PF_Scheduler -r 0..99999 > /dev/null 2>&1
```

### 2 - Analyze the results
After running the simulations, navigate to the analysis folder:
```bash
$ cd analysis
```

Run the Python scripts to process and convert the results:

```bash
# Analyze raw simulation data in a folder
$ python sca_analysis.py FOLDER_NAME

# Convert analyzed data to JSON format
$ python sca_to_json.py FOLDER_NAME

# This does the same as sca_analysis.py but is faster and JSON files are easier to store
$ python json_analysis.py FILE_NAME
```