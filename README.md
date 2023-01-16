# ConflictMissesSimulator
This is a simulator for evaluating conflict misses with hit rate in hybrid memory systems. It migrates data at a way similar to the paper [CAMEO](https://doi.org/10.1109/MICRO.2014.63) but can switch to different granularities. 

# Build and debug

Before starting to build or debug this project, you might need to be familiar with [the visual studio code tutorial](https://code.visualstudio.com/docs/cpp/config-linux).

## Build
Build methods are explained below.

### 1. Visual Studio Code-based method.
- You may need to modify the compiler's path in the `tasks.json` file in the `.vscode` directory.
- Click `Run Build Task` in the `Terminal` tab.

## Debug
Debug methods are explained below.

### 1. Visual Studio Code-based method.
- You may need to modify the debugger's path in the `launch.json` file in the `.vscode` directory.
- Click `Start Debugging` in the `Run` tab.


# Run simulation
```
$ [EXECUTION]
```
where [EXECUTION] is the executable file's name, such as ./bin/entry.exe.