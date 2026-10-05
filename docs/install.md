# Ubuntu Installation Instructions

## 1. Install prerequisites for ROOT
- From: https://root.cern/install/dependencies/
- Used the one-liner for Ubuntu, then installed all optional packages with the second one-liner.

## 2. Build ROOT from source
- Instructions from: https://root.cern/install/build_from_source/
- Using ROOT 6.32.04: https://root.cern/releases/release-63204/

```bash
tar -xvf root_v6.32.04.source.tar.gz
mv root-6.32.04 root-6.32.04-src
mkdir root-6.32.04-build
mkdir root-6.32.04
cd root-6.32.04-build/
cmake -DCMAKE_INSTALL_PREFIX=../root-6.32.04 ../root-6.32.04-src
```

- Use `nproc` to check number of cores. Here, mine is 24.

```bash
cmake --build . --target install -j24
```

- Using `-j<result of nproc>`.
- Added sourcing of `thisroot.sh` to shell login script (here `.bashrc`):

```bash
gnome-text-editor ~/.bashrc
```

- Add this to the bottom:

```bash
source ~/dev/root-6.32.04/bin/thisroot.sh
```

## 3. Install Geant4 11.3.0

Install Geant4 11.3.0 source from: https://github.com/Geant4/geant4/releases

Following instructions from: https://geant4-userdoc.web.cern.ch/UsersGuides/InstallationGuide/BackupVersions/V11.3b/html/installguide.html#on-unix-platforms

```bash
mkdir geant4-11.3.0-build
mkdir geant4-11.3.0
cd geant4-11.3.0-build/
cmake -DCMAKE_INSTALL_PREFIX=../geant4-11.3.0 -DGEANT4_INSTALL_DATA=ON ../geant4-11.3.0.beta
make -j<nproc output>
make install
```

- Add to `.bashrc`:

```bash
source ~/dev/geant4-11.3.0/bin/geant4.sh
```

## 4. Set up GitHub

```bash
sudo apt install git openssh-client
git config --global user.name "Sam Hedges"
git config --global user.email "schedges@vt.edu"
ssh-keygen -t ed25519 -C "schedges@vt.edu"
```
- I accepted the default location and no passphrase.

```bash
eval "$(ssh-agent -s)"
ssh-add ~/.ssh/id_ed25519
```

- Assuming you used the default path.

```bash
cat ~/.ssh/id_ed25519.pub
```

- Copy and paste this to:
  **GitHub → Settings → SSH and GPG keys → New SSH Key**
- Set a name for the machine.

## 5. Install `paleo-bg-sim`

> Note: optionally install HDF5 support first with:

```bash
sudo apt install libhdf5-dev
```

In the folder where you keep your code:

```bash
git clone git@github.com:NSF-GCR-MDDM/paleo-bg-sim.git
mkdir palpe-bg-sim-build
cd paleo-bg-sim-build
cmake ../paleo-bg-sim
make clean
make -j<nproc output>
```

## 6. Set up a Visual Studio Code IDE

- Download the `.deb` version from: https://code.visualstudio.com/
