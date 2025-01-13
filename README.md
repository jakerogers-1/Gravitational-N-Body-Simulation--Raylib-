# 🌌 N-Body Simulation in 2D Using Raylib
  
This is a real time N-body simulation in 2D that uses **Raylib** for visualising. 
Multiple simulation types (naive, Barnes Hut) and advanced numerical integration methods are included.

---

## Features

- **Simulation Types**: Currently including naive and Barnes Hut.
- **Numerical Integrators**: Such as Euler, Velocity Verlet, and 4th order Yoshida.
- **Gravitational Scenarios**: With three predefined scenarios, including a solar system model, a binary orbit model, and a randomly populated universe.
- **Periodic Boundary**: To allow diverse simulation types and more sophisticated models
- **Real Time Modification**: The flexible design allows the user to change types, integrators, and scenarios on the fly.

---

## Future Work

- **Particle Mesh (PM) and P3M Methods**: To simulate large-scale structure
- **Web Assembly Compilation**: To allow for online hosting and interaction
- **Body Creation Menu**: So that users can add their own bodies to an existing scenario

---

## Installation and Set-up

main.exe can be used to launch the application  

To make modifications to the code itself, please open main.c in VS Code and use F5 to compile