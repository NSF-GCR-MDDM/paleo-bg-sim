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
This code handles the GEANT4 simulation of particle interactions in paleo detectors and in the surrounding rock,
trying to be general purpose. There will also be some code to handle replacing ion recoils in specific target
volumes with processed tracks from TRIM.

## Installation

Full installation instructions can be found here:[Installation Instructions]](docs/install.md)

## Running

```bash
./particleProduction <macro file>
```

The macro file arguments are documented here: [Macro Command Options](docs/macro_commands.md). In the newest vesion
of the code the geometry is specified through a separate macro linked to the main. Details on that format can be found
here: [Geometry Command Options](docs/geometry_commands.md)

command-line options:
- `--seed <#>` : Uses the passed in number as the random seed. Otherwise uses system time to generate one.
- `--outputFile <path/and/filename.root>` : Sets the output file name, otherwise defaults to outputFiles/output.root in current folder.

## Analysis

Details on analyzing the produced output will be found here.

## Developing

* See [Coding Style and Overview](docs/coding_overview.md) for details on the structure of the code and recommended best coding practices.
* See [Adding a Custom Source](docs/custom_source.md) for details on how to add your own sources.

