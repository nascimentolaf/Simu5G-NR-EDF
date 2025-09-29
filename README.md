# NR-EDF Simu5G Extension

This repository provides an NR-EDF (New Radio – Earliest Deadline First) extension for Simu5G OMNeT++ library. It introduces new scheduling and analysis features tailored for 5G simulations with real-time traffic constraints.
Refer to [README_SIMU5G.md](./README_SIMU5G.md) for the original Simu5G 1.3.0 documentation.

## Core contributors
- Alaf Nascimento (alaf.nascimento@telecom-paris.fr)
- Philippe Martins (philippe.martins@telecom-paris.fr)
- Samuel Tardieu (samuel.tardieu@telecom-paris.fr)
- Laurent Pautet (laurent.pautet@telecom-paris.fr)

## NR-EDF Simu5G Features
- NR-EDF Scheduler
- Missed Deadlines Counter for periodic and sporadic traffic
- Python scripts for:
  - Converting `.sca` files into `.json` files
  - Analysing `.sca` and `.json` files (JSON is more storage-efficient)

## Installation
To install our extension, follow these steps:

1. Install **OMNeT++**.  
2. Inside the OMNeT++ directory, create a folder named `workspace`.  
3. Install **INET** and **Simu5G** inside the `workspace` folder.  
4. Copy the contents of this repository into the `Simu5G` folder, replacing the existing files. **Do not delete the original `Simu5G` folder**, as it contains object files that are still needed.
5. Follow the rest of this README to learn how to use the new features introduced in this extension.  

> **Note:** This software has been tested on **Linux Mint 21.2**, but it should work on any distribution supported by the original Simu5G.

### Dependencies
This version requires:  
- **OMNeT++ 6.1.0**  
- **INET 4.5.4**
- **Simu5G 1.3.0**



## How to use this extension
We assume you have your folders organized as follows:

```bash
~/omnetpp-6.1/
├── workspace/
│   ├── inet4.5/
│   └── simu5g/
```

Configure the environment:
```bash
# Configures current OMNeT++, INET, and Simu5G environments
source ./config2.sh

# Go to the SCADA tutorial simulations folder
$ cd simulations/NR/tutorial_scada

# Set up the configuration for the extension
$ ./config.sh
```

### 1 - Run the simulation scripts

Run the simulations with different configurations and/or schedulers. The output is redirected to /dev/null to keep the terminal clean:

```bash
$ opp_runall -j"$(nproc)" ./run -u Cmdenv -f OMNETPP_FILE -c SCHEDULER -r 0..99999 > /dev/null 2>&1
```

| SCHEDULER Options | Description                            |
| ----------------- | -------------------------------------- |
| `EDF_Scheduler`   | New Radio Earliest Deadline First |
| `PF_Scheduler`    | Proportional Fair                      |

| OMNETPP_FILE Options | Purpose                                                                 |
| -------------------- | ----------------------------------------------------------------------- |
| `omnetpp_dim.ini`    | Resource block dimensioning for simulations with only real-time packets |
| `omnetpp_ue_den.ini` | Study of schedulability for different densities of real-time services   |
| `omnetpp_mixed.ini`  | Throughput and schedulability analysis for mixed packet types           |


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