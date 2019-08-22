#### The integration of the Matlab-Simulink auto-generated code(C/C++) on linux

### BACKGROUND
The Matlab-Simulink is a powerful software which could help the engineers to develop rapidly the software in the ECU of the vehicle.

If they want to integrate the algorithm directly in a platform, for example, like `Linux`, the Matlab could also generate an embedded-code version.

But if the algorithm need to use some peripheral equipment, such as CAN which is the most common communication method in a vehicle. You need to modify the code each time after the generation, so that the peripheral equipment could be used.

### DESCRIPTION
In order to modify as less as possible the code, the project gives a solution. And the socketcan could be used.

### BUILD
* `make` (libSimulinkSDK.a will be generated in the folder lib after this step)
* `make clean`
* The file `api.h` will help you to use this solution.
* An example is given at ![here]()