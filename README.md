# Low Cost MRR For Malaria Diagnosis

This project is a low-cost, low complexity version of the instrument presented by Peng, Chen, and Han in [Development of miniaturized, portable magnetic resonance relaxometry system for point-of-care medical diagnosis](https://aip.scitation.org/doi/citedby/10.1063/1.4754296), used by Peng et al. in [Micromagnetic resonance relaxometry for rapid label-free malaria diagnosis](https://www.nature.com/articles/nm.3622). 

This tool attempts to significantly lower the cost of an instrument capable of diagnosing malaria and other diseases by stripping the researchers' design of all functionality that is not absolutely necessary for this specific application. Notably, this is done by:
1. Modifying the magnet holder so that the separation between the magnets on either side of the sample can be adjusted. This allows the larmor frequency for proton MRR to be tuned to an easily obtaiable frequency. 
2. Generating the CPMG pulse train and the 20MHz carrier with a cheap microcontroller (ATMEGA328P)
3. Replacing COTS RF components with custom implementations using cheap passives and transistors wherever possible
4. Implementing the receiver with tuned filters and peak detect circuits

## New Hardware
