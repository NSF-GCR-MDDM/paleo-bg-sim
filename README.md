# PaleoSim

Code for simulating interactions interactions in paleo detectors

---

## Table of Contents
- [About](#about)
- [Installation](#installation)
- [Compiling and Running](#Running)
- [Analysis](#Analysis)
- [Developing](#Developing)

---

## About
This GEANT4-based simulation models particle interactions in paleo-detectors and the surrounding rock. It is intended as a general-purpose framework, capable of simulating background fluxes incident on detectors as well as tracking ion recoils within detector materials.

The codebase is under active development. As such, no guarantees are made regarding accuracy, completeness, or stability. Input formats, macro structures, and geometry definitions may change without notice, and backward compatibility is not assured.

We are releasing this repository as a starting point for others interested in paleo-detector simulations. While we are not currently seeking external contributors, we welcome bug reports and feedback.

## Installation
As a design philosophy, we have tried to minimize external dependencies. As a result we have external generators (MARLEY, CRY, MUTE) generate ROOT or text files that are then sampled for event generation within Geant4. Similarly external VRML viewers (such as geviewer) are the preferred way of visualizing geometry.

Testing has been done with ROOT v6.32.04 and Geant4 v.11.3.0. 

Full installation instructions will located here once written:[Installation Instructions](docs/install.md)

## Running

```bash
./particleProduction < macro file >
```

The macro file arguments are documented here: [Macro Command Options](docs/macro_commands.md). The geometry is specified through a separate macro linked to the main. Details on that format can be found here: [Geometry Command Options](docs/geometry_commands.md)

command-line options:
- `--seed <#>` : Uses the passed in number as the random seed. Otherwise uses system time to generate one.
- `--outputFile <path/and/filename.root>` : Sets the output file name, otherwise defaults to outputFiles/output.root in current folder.

## Analysis

Details on analyzing the produced output will be found here.

## Developing

* See [Coding Style and Overview](docs/coding_overview.md) for details on the structure of the code and recommended best coding practices (TODO)
* See [Adding a Custom Source](docs/custom_source.md) for details on how to add your own sources.

