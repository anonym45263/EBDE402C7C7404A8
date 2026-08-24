# First Tree-like Quantum Data Structure: Quantum B+ Tree

This repository contains the code for the submission *"First Tree-like Quantum Data Structure: Quantum B+ Tree"*.

## Requirements

- Linux environment with GCC
- CMake (>= 3.17)

## Usage

1. Download this repository.

2. Change the working directory into `src/`.

3. Build with the `cmake` command.
   - This will generate the Makefile of this project.

4. Compile with the `make` command.
   - This will generate all the executables under `src/` (e.g., `Brightkite-v-N`, `IMDb_search-v-N`, `TPCH_join1`).

5. Create an output directory for result files written by the programs:

   ```bash
   mkdir -p plot
   ```

6. Download the datasets into `src/`:

   - **Brightkite** (SNAP):
     - https://snap.stanford.edu/data/loc-brightkite_totalCheckins.txt.gz
     - Unzip and rename to `brightkite_totalCheckins.txt` (this is the filename expected by `Brightkite-aug`).

   - **IMDb** (non-commercial datasets):
     - https://datasets.imdbws.com/title.basics.tsv.gz
     - https://datasets.imdbws.com/title.ratings.tsv.gz
     - Unzip and rename to `IMDb-title.basics.tsv` and `IMDb-title.ratings.tsv`.

   - **TPC-H**:
     - Generate tables with the official TPC-H `dbgen` tool.
     - Place the following files in `src/`, named as:
       - `TPCH-customer.tbl`
       - `TPCH-lineitem.tbl`
       - `TPCH-nation.tbl`
       - `TPCH-orders.tbl`
       - `TPCH-part.tbl`
       - `TPCH-partsupp.tbl`
       - `TPCH-region.tbl`
       - `TPCH-supplier.tbl`

7. Prepare derived input files (if needed):

   - Brightkite scaled inputs used by the Brightkite query programs:

     ```bash
     ./Brightkite-aug
     ```

     This generates `brightkite_ts_data_01.txt`, ..., `brightkite_ts_data_16.txt`.

   - IMDb processed input used by the IMDb query programs:

     ```bash
     ./IMDb-process
     ```

     This generates `IMDb-processed.txt`.

8. Run experiments. Parameters such as dataset size `N`, branching factor `B`, and selectivity are set inside each source file; executables take no command-line arguments.

   **Example (Brightkite, vary `N`):**

   ```bash
   ./Brightkite-v-N
   ```

   The program prints a table of IO costs and execution times for the classical and quantum B+ trees (build and query), for example:

   ```text
   N                                          4096               8192              ...
   ----------------------------------------------------------------------------
   IO_C Query Classical                    ...
   IO_C Query Quantum                      ...
   Time Query Classical                    ...
   Time Query Quantum                      ...
   ```

   Tabular results are also written to `plot/Brightkite-v-N.sta`.

   **Example (IMDb search, vary `N`):**

   ```bash
   ./IMDb_search-v-N
   ```

   The program reports IO cost and time for classical scan / quantum / hybrid methods, and writes `plot/IMDb_search-v-N.dat`.

   **Example (TPC-H join-order / counting scenario):**

   ```bash
   ./TPCH_join1
   ```

   The program evaluates classical counting, quantum counting, and proportional estimation under different selectivities, prints a summary table (IO cost and time), and writes `plot/TPCH_join1.dat`.

## Executables (overview)

| Executable | Description |
|---|---|
| `Brightkite-aug` | Augment / sample Brightkite timestamps into scaled inputs |
| `Brightkite-v-N` / `Brightkite-v-k` / `Brightkite-v-B` | Brightkite range-query experiments (vary `N`, selectivity, or `B`) |
| `Brightkite-dyn-v-N` | Dynamic Brightkite insert/delete experiment |
| `IMDb-process` | Preprocess IMDb TSV files into `IMDb-processed.txt` |
| `IMDb_search-v-N` / `IMDb_search-v-k` | IMDb search-query experiments |
| `IMDb_maximum-v-N` / `IMDb_maximum-v-k` | IMDb maximum-query experiments |
| `TPCH_join1` / `TPCH_join2` | TPC-H join-order / join-algorithm selection experiments |
| `TPCH_join1_rand` / `TPCH_join2_rand` | Randomized variants of the TPC-H experiments |
