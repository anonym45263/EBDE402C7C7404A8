# First Tree-like Quantum Data Structure: Quantum B+ Tree
This repository contains the code, data and technical report for submission "First Tree-like Quantum Data Structure: Quantum B+ Tree".

## Requirements
- Linux Environment with GCC

## Usage

1. Download this repository

2. Change the working direction into src/.

3. Build with `cmake` command.
   - This will generate the Makefile of this project.

5. Compile with `make` command.
   - This will generate all the executables.
   - The executable is in the form of "Dataset-x" where Dataset is either Brightkite or gowalla, and x is a number indicating the type of query:
       - 1: Static quantum range query
       - 2: Dynamic quantum range query
  
6. Download the datasets into src/:
   - https://snap.stanford.edu/data/loc-brightkite_totalCheckins.txt.gz
   - https://snap.stanford.edu/data/loc-gowalla_totalCheckins.txt.gz

7. Run the tests with `./Dataset-x N B s` according to the dataset and the type of query, where N is the specified dataset size, B is the branching factor and s is the query selectivity.
   - For example, command `./Brightkite-1 1000000 8 0.05` runs the static quantum range query with dataset size = 1M, branching factor = 4 and selectivity = 5%.

8. The program will output two numbers, the first is the number of IOs of the traditional data structure (e.g., the traditional B+-tree), and the second is the number of IOs of our quantum data structure (e.g., the quantum B+ tree).
