# Project 3: Results and Analysis

## Submission Information

**Andrew Prata, (https://github.com/apratajr/AdvancedComputerSystems/)**

*ECSE 4320: Advanced Computer Systems* Fall 2023

Due: 25-Oct-2023

## What Is Covered Here?
This is a report detailing the experimental process of characterizing the performance of a Solid State Drive (SSD) using the Linux command-line utility Flexible IO Tester (FIO). Experimental deliverables are as follows:

`Latency` and `Throughput` must be determined for combinations of
    
    1. Data Access Size: 4KB, 16KB, 32KB, and 128KB
    2. Read/Write Intensity Ratio: RO, WO, and 70:30 R:W
    3. I/O Queue Depth: 0-1024 Tasks

The FIO utility is extremely powerful, and will be used to extract the latency and throughput data under these varied conditions. To see how this was done, please take a look at the section **Data Aquisition**. For the subsequent presentation and analysis of the data, please see **Results and Analysis**.

## Data Aquisition
### Setup
To start, I focused on getting a native Linux machine up and running. This would ensure that FIO would operate as intended and without any trouble that might result from running the OS in a virtual machine, or with the Windows Subsystem for Linux.

The selected machine is an old Lenovo Thinkpad T410, into which I installed a SATA SSD. The drive is an APPLE SSD SM256E, with capacity 251 GB. The selected operating system is Ubuntu 22.04.3 Desktop, with a minimal installation.

### Testing
Testing was done using FIO with a datafile on the system disk. This was convenient, as it avoided creating a new partition just for testing.

To test different data access sizes, the FIO parameter `--bs` (or `--blocksize`) was adjusted.

## Results and Analysis
