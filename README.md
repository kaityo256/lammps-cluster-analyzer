
# LAMMPS Cluster Analyzer

[![License: MIT](https://img.shields.io/badge/License-MIT-yellow.svg)](LICENSE)

`cluster-analyze` is a standalone C++ code that analyzes LAMMPS trajectory files (`lammpstrj`) and performs local density analysis and clustering for atoms.

### Processing Overview

* Reads a LAMMPS trajectory file in `lammpstrj` format  
* For each frame:
    * Computes the local density of atoms with `Type == 1`
    * Classifies cells with density above the threshold as liquid phase
    * Performs clustering of liquid-phase cells based on adjacency (site percolation)
    * Outputs the number of clusters to standard output
    * Writes the **cluster size distribution** to `clustersize.<FRAME>.dat`, where `<FRAME>` is the **zero-padded frame index** (e.g., `0000`, `0001`, `0002`, …)
    * Writes the **local density field** in VTK format to `density.<FRAME>.vtk`, where `<FRAME>` is the **zero-padded frame index**
* Writes the **time evolution of the number of clusters** to   `cluster_count.dat`

## Dependencies

All dependencies are header-only libraries included via `external/`:

| Library | Purpose | License |
|----------|----------|----------|
| [lammpstrj-parser](https://github.com/wtnb-appi/lammpstrj-parser) | LAMMPS trajectory reader | MIT |
| [cxxopts](https://github.com/jarro2783/cxxopts) | Command-line parser | MIT |

## Build Instructions

This repository includes a GNU Makefile. You can build the code with the following commands:

```bash
git clone --recursive https://github.com/kaityo256/lammps-cluster-analyzer.git
cd lammps-cluster-analyzer
make
```

After a successful build, the executable `cluster-analyze` will be generated.

The external library `lammpstrj-parser` is managed as a Git submodule.  
Be sure to clone the repository with the `--recursive` option, or initialize it manually as follows:

```sh
git submodule update --init --recursive
```

## Command-line Options

This program takes a LAMMPS trajectory file (`.lammpstrj`) as input and performs cluster analysis.  
The basic usage is as follows:

```bash
./cluster-analyze [options] filename
```

### Positional Argument

| Argument | Description |
|---|---|
| `filename` | LAMMPS trajectory file (`.lammpstrj`) to be analyzed |

---

### Option List

| Short | Long | Type | Default | Description |
|---|---|---|---|---|
| `-m` | `--mode` | string | `bubble` | Specifies the analysis mode. Either `bubble` or `droplet` can be selected. |
| `-s` | `--mesh-size` | double | `2.0` | Mesh size used for the analysis. |
| `-t` | `--density-threshold` | double | `0.3` | Density threshold for each cell. Cells with density above this value are treated as liquid phase, and those below as gas phase. |
| `-v` | `--vtk` | bool | `false` | If specified, outputs the analysis results in VTK format.  **The `=` sign is required for boolean options** (e.g. `-v=true` or `--vtk=true`).|
| `-h` | `--help` | – | – | Displays usage information and the list of available options. |

---

### License

This repository is released under the MIT License.  
See the [LICENSE](LICENSE) file for details.
