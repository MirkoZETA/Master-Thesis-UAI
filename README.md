# Multi-band EON Thesis
Comparative analysis of multi-band optical networks focusing on Elastic Optical Networks (EON) in C+L bands versus fixed-grid networks in C+L+E/S bands. Research on network capacity, spectrum efficiency, and performance optimization.

## Key Findings & Conclusiones

1. Multi-band extended configuration (C+L+S/E) demonstrated superior performance across all scenarios compared to MB-EON C+L.
2. Optimal band expansion choice depends heavily on network topology characteristics:
    * For long-distance networks (NSFNet): C+L+S configuration proved most effective.
    * For shorter distance networks (UKNet/EuroCore): C+L+E configuration achieved best results.
3. EON showed competitive performance despite bandwidth limitations:
    * Achieved up to 110% capacity increase in incremental traffic scenarios.
    * Spectral efficiency partially compensated for lower total bandwidth.
4. Notable limitations identified in continuous MB expansion:
    * Some bands provide practically no extra usable bandwidth in long-distance topologies.
    * In long distance topologies, after S-band expansion, EON implementation or SDM adoption may be more viable alternatives.

## Overview
This research project conducts extensive simulations comparing different multi-band optical network configurations:
- C+L band (50 GHz channels)
- C+L band (12.5 GHz channels)  
- C+L band (6.25 GHz channels)
- C+L+S band (50 GHz channels)
- C+L+E band (50 GHz channels)

The study evaluates network capacity, spectrum efficiency, blocking probability (BP), and bandwidth blocking probability (BBP) across multiple scenarios.

## Project Structure
```
project/
├── src/                        # Source code files
│   ├── profiles/              # Traffic demands and bit rates
│   ├── topologies/            # Network topology definitions
│   ├── scenarios/*.cpp        # Main simulation files (containing core logic)
│   ├── functions.hpp          # Auxiliary functions
│   ├── simulator_incremental.hpp  # Flex Net Sim library for incremental traffic
│   └── simulator.hpp          # Flex Net Sim library for dynamic traffic
├── scripts/                   # Execution scripts (run from root directory)
│   ├── CL_50_script.sh       # C+L 50GHz simulation
│   ├── CL_125_script.sh      # C+L 12.5GHz simulation
│   ├── CL_625_script.sh      # C+L 6.25GHz simulation
│   ├── CLS_script.sh         # C+L+S simulation
│   ├── CLE_script.sh         # C+L+E simulation
│   └── INCREMENTAL_script.sh # Capacity analysis
├── results/                   # Simulation results
├── results_original/          # Published results
├── temp/                      # Temporary executables
└── README.md
```
## Usage Instructions

### Running Simulations
All scripts must be executed from root directory.

```bash
cd multiband-eon-thesis
chmod +x ./scripts/[script_name].sh
./scripts/[script_name].sh
```

Each script:
- Runs simulations for 3 topologies
- Uses 2 algorithms per topology
- Generates 6 result files
- Performs 4 iterations per simulation

Note: Full simulation sets may take several days. Incremental scenario takes ~1 hour.

### Results Format

#### Dynamic Traffic Scenarios
Output format (per Erlang value):
```csv
ERLANG  BP  ERROR   ERROR\\   BBP
```

Each line contains:
- ERLANG: Traffic load value
- BP: Blocking Probability
- ERROR: Margin Errors
- BBP: Bandwidth Blocking Probability

Example for two Erlang values:
```
2500    0.025196    0.00030717    0.00030717\\    0.0252918
3000    0.0654249   0.000484649   0.000484649\\   0.0453274
```
The number of results varies by topology due to different Erlang ranges. For example, NSFNet topology results (one iteration):
```
1000	5.99999e-06	5.17083e-06	5.17083e-06\\  7.93334e-06
1250	0.00297	0.000106671	0.000106671\\  0.00218986
1500	0.02224	0.000289027	0.000289027\\  0.0121526
1750	0.049272	0.000424207	0.000424207\\  0.0265783
2000	0.0767969	0.000521878	0.000521878\\  0.0427261
2250	0.101892	0.000592901	0.000592901\\  0.0589976
2500	0.123355	0.000644522	0.000644522\\  0.0745447
2750	0.144054	0.000688229	0.000688229\\  0.0894169
3000	0.163115	0.000724147	0.000724147\\  0.103586
3250	0.181113	0.000754803	0.000754803\\  0.117129
3500	0.198949	0.000782434	0.000782434\\  0.130148
3750	0.21533	0.000805643	0.000805643\\  0.142622
4000	0.231126	0.000826225	0.000826225\\  0.154609
```

#### Incremental Traffic Scenario
Output format:
```
HEADER SCENARIO
, NCONECTIONS) +- (ERROR, ERROR)
, TOTALBITRATE +- (ERROR, ERROR)
C: ,NConnextionsInC)
L: ,NConnextionsInL)
E: ,NConnextionsInE)
S: ,NConnextionsInS)
C: ,TotalBitRateInC)
L: ,TotalBitRateInL)
E: ,TotalBitRateInE)
S: ,TotalBitRateInS)
```
Example:
```
FirstFit - EuroCore - CL_625
, 6160.93) +- (50.48, 50.48)
, 1.53588e+06) +- (11488.4, 11488.4)
C: ,1195)
L: ,4965)
E: ,0)
S: ,0)

C: ,297176)
L: ,1238703)
E: ,0)
S: ,0)
```
#### BBP Analysis
After running BP scripts, run Jupyter notebook: `./results/compute_BBP.ipynb`. This will calculate the BBP values based on the BP iterations.

Output format:
```
BBP RESULTS
ERLANG      BBP     ERROR       ERROR\\
```

Example:
```
2500    0.025260    0.000325    0.000325\\
3000    0.045405    0.000178    0.000178\\
```

## Dependencies
- C++ compiler (g++)
- Jupyter Notebook (for BBP analysis)
- Python 3.x with required packages:
    - numpy
    - scipy
    - matplotlib

## Installation
1. Clone repository
2. Ensure dependencies are installed
3. Create required directories:
```bash
  mkdir -p temp results
```

## Documentation
For detailed methodology and results analysis, refer to the associated thesis document.

## Future Work
Potential areas for further research:
1. Compare scenarios incorporating QoT-E for GSNR band estimation
2. Incorporate fragmentation avoidance strategies for EONs
3. Include implementation cost metrics (cost per bit)
4. Analyze determining factors in topology-based band expansion selection
5. Study the relative impact of route length vs other topological characteristics

## Acknowledgments
This work was supported by Universidad Adolfo Ibañez's Faculty of Engineering and Sciences. Special thanks to thesis advisors Danilo Bórquez Paredes and Gabriel Saavedra Mondaca for their guidance and support.