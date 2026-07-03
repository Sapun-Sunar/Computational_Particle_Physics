# Computational Particle Physics: Standard Model Reconstruction

## The Objective
This repository is a programmatic dissection of the Standard Model of particle physics. It contains high-performance C++ architectures designed to extract the fundamental constants of the universe from raw, petabyte-scale collision data provided by the CERN CMS detector.

This is not a blind statistical search. It is the rigorous mathematical resurrection of quantum resonances—forcing decayed, microscopic ghosts back into existence through relativistic tensor algebra and advanced Chi-Squared ($\chi^2$) minimization.

## Repository Architecture

The physics dictates the code structure. Particles are segregated by their fundamental forces and decay mechanics:

### `/z_boson` (The Weak Force Mediator)
* **Mechanics:** Reconstructs the $Z$ Boson via the invariant mass of its daughter muons ($\mu^+ \mu^-$).
* **Mathematics:** Employs a 7-parameter Linear Superposition fit. Uses a Crystal Ball function to mathematically correct for asymmetric Bremsstrahlung radiation losses in the detector silicon, layered over an Exponential Drell-Yan background.
* **Scale:** Utilizes ROOT's Implicit Multi-Threading (IMT) to process millions of collision events simultaneously.

### `/w_boson` (The Transverse Ghost) - *In Development*
* **Mechanics:** The $W$ Boson decays into a lepton and a Neutrino. Because neutrinos do not interact with the detector, they cannot be measured directly. 
* **Mathematics:** This module calculates Missing Transverse Energy (MET) across the entire detector grid to track the invisible neutrino's momentum vector, allowing for the calculation of the W Boson's Transverse Mass.

### `/higgs_boson` (The Mass Giver) - *In Development*
* **Mechanics:** Isolates the Higgs field ripple via the incredibly rare diphoton ($\gamma \gamma$) decay channel. 
* **Mathematics:** Requires massive background suppression algorithms to separate the true Higgs signal from the overwhelming noise of standard electromagnetic interactions.

*(Note: There is no directory for isolated quarks. Due to the exact laws of Quantum Chromodynamics (QCD) and Color Confinement, attempting to isolate a single quark is physically impossible and results in hadronic jets.)*

## Technical Arsenal
* **C++17 / C++20**: Bare-metal performance for petabyte-scale iteration.
* **CERN ROOT**: `RDataFrame` for execution pipelines, `TF1` for multi-dimensional optimization, `ROOT::Math::Vector4D` for Minkowski spacetime kinematics.