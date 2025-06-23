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
This GEANT4 simulation is designed to model particle interactions in paleo detectors and in the surrounding rock. The code is designed to be general purpose, from modeling backgrounds incident on detectors to recording ion recoils inside the detectors. It is currently a work in progress and the authors and contributors make no guarantees of its accuracy. The authors may change the format of required input files as work progresses and make no guarantee of backwards compatibility with existing macros and geometry files.

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

