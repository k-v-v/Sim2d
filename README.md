# Sim2d
A 2d multi backend Navier-Stokes solver. 

**CPU solver demonstration:** https://streamable.com/7seli

**Solver backends**
- [x] CPU single core
- [ ] CPU multi-core with c++ threads
- [x] CUDA (**Simulation is unstable.**)
- [ ] Allow dynamic switching
- [ ] ? OpenGL 4 Compute shaders
- [ ] ? OpenCL 

**Task list**

- [x] Create Application and Solver skeletons 
- [x] Set up a simple window
- [x] Render a blank screen with Opengl
- [x] Set up opengl texture to be rendered
- [x] Implement basic mouse input   and drawing
- [x] Implement a solver
- [x] Render output to screen
- [x] Implement another solver
- [ ] ?Set up solver testing and consistency checking by comparing 2 parallel solver simulations by showing the difference
- [ ] ?Windows platform support 

**GUI**
- [ ] Show FPS and backend
- [x] Modify simulation speed
- [x] Restart simulation without clearing boundry conditions
- [x] Draw boundry conditions
- [x] Clear boundry conditions
- [x] Set up fluid direction
- [ ] Resize solver grid and apply resize of restart
- [ ] Show fps graph



Code based on Numerical Simulation in Fluid Dynamics: A Practical Introduction (Griebel, Dornseifer, Neunhoeffer).
https://doi.org/10.1137/1.9780898719703
