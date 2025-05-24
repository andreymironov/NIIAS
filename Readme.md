# NIIAS

## Development environment

Ubuntu 24.04.2 LTS, 
Qt Creator 13.0.0 based on Qt 6.4.2, 
C++17/STL

## Clone, prepare to build, build and run (CLI)
 1. Clone this repository to local folder
 2. Enter cloned folder
 3. Run `setup.sh` in order to update system libraries, build essentials and Qt libraries to the latest version (use `chmod` command to make this shell script executable, if required)
 4. Run `build.sh` in order to execute `qmake6` (this will generate Makefile on NIIAS project) and `make`
 5. Run `NIIAS` compiled executable

## Clone, prepare to build, build and run (GUI)
 1. Clone this repository to local folder
 2. Enter cloned folder
 3. Run `setup.sh` in order to update system libraries, build essentials and Qt libraries to the latest version (use `chmod` command to make this shell script executable, if required)
 4. Install or update QtCreator, if required (run `sudo apt-get install qtcreator`). Qt version 6.4 should be added to Qt versions in your QtCreator settings in order to open this project (use `/usr/bin/qmake6` path)
 5. Run QtCreator and open `NIIAS.pro` project
 6. Compile and run

## Running NIIAS
The app will automatically load *Railroad Sensors Layout* (RSL) from `sensors.conf` file at startup (`qmake6` will copy/deploy this file alongside with executable wherever path you'll build the project).

The app will present sensor layout from loaded RSL file visually, at startup, in the `View tab`.

Next, open *Sensor Data File* (SDF) via app's menu `File -> Open...` . Example file `/sensor data/test.txt` is attached to this repository.

Use `Result tab` to explore collected sensor data in per-sensor basis, in relative and absolute time, local chassis and length formulae, average velocity and its deviation; as well as global (averaged) estimated chassis and length formulae.

Use `View tab` to play animation and explore train passing sensors, realtime sensor counter values, train's head position and velocity.

See `screenshot.png` to preview the app.
