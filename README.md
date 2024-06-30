# Three body simulator

This project calculates trajectories of 3 (or more) bodies under gravitational force.

## Motivation

This project was created by [McGyv246](https://github.com/McGyv246), [CarloDegasperi](https://github.com/CarloDegasperi) and [lorenzob0nati](https://github.com/lorenzob0nati) as school project for physics degree at Università degli studi di Trento.

## Usage guide

This program needs an input file with the correct structure. See [input_1.dat](input_1.dat) as example.

These are the meaning of the headers:
- N: (integer) number of the bodies
- G: (double) gravitational constant
- dt: (double) integration interval
- tdump: (integer) how many integrations the program should do before printing the state of the system
- T: (integer) total number of integrations the program should do

Note that the program has been built to work with an arbitrary number of bodies AND an abitrary number of dimensions. Set up your input file accordingly (to edit the number of dimensions the program works with you will also need to update the SPATIAL_DIM macro in [main.c](main.c) file).

So yes, if for some reason you need to simulate how 10 planets would behave in a 10-dimensional space, this program can do that.

Compile and run with these commands (insert correct input file name):
```
$ gcc -std=c99 -Wall -Wpedantic -O3 main.c integrator.c geom.c -o main.exe -lm
$ ./main.exe input_1.dat
```

Output will be printed in 2 files:

- `traj.dat` that will contain the trajectories for each instant
- `energies.dat` that will contain the energies for each instant

## Structure

- [geom.c](geom.c) contains geometric functions
- [integrator.c](integrator.c) contains integration function
- [main.c](main.c) orchestrates execution of the whole program (it reads input, executes integrations, prints output etc.)

Comments in [notes.md](notes.md) file and in the code served as clarification for the person who graded the project and should not be considered. Note that docstrings are in written in italian.

[correzione](correzione) folder contains the graded version of the code.

## Updates

This project will no be updated. If you want to modify this project you can fork it.
