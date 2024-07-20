# PlanetGen
This is a basic procedural planet generation system written in c++ using OpenGL through Freeglut.

# Current State

  - Sphere generation starts from a cube with side length being a power of 2, this prvents precision errors on CPU side for planets up to **32.000 km** radius (we will consider a 1.0 unit as a meter) using 32 bit floating point values.
  - Each face of the cube is the root of a quad tree. These are divided into 4 squares of equivalent size based on a formula of side length and distance to camera, until maximum level of detail is reached, which is calculated based on planet radius. As result the smallest squares will be **~10cm * 16**, why * 16 is explained in next paragraph. Also at this stage a little optimization is implemented such as squares that are the other side of the planet are not divided more than several times.
  - Next comes tessellation control shader. Here is applied horizon culling (squares that are behind the horizon will be tessellated only once), squares that passed the test get inner tessellation level 15, which means they will become a grid of 15x15 squares plus one segemnt from outer level used for connection to other squares, this us the last 3 levels of detail. Tessellation outer level is calculated based on maximum and minimum level of detail present in current frame. A resulted square will look like this:
<br>![Could not find image](https://github.com/MrDebuggins/PlanetGen/assets/96007801/f44d3816-29f5-4676-9ab9-2e2683f803e8)<br>
  - The next steps take place in tessellation evaluation shader.
  - Cube resulted from previous step is transformed into a sphere of radius R as follows: each vertex is normalized (we obtain a sphere of radius 1) then its coordinates are multiplied by R. The problem here is that we perform operations on floating point values with significant difference in exponent, so for now these calculations are done on 64 bit values. This way we get rid of precision errors, but with a drawback in performance, for example on **Nvidia RTX 3000 series GPU the ratio between 64 bit and 32 bit ALU's is 1/64**, which makes about **50 FPS** difference. So in next versions i will probably emulate 64 bit operations using two 32 bit values.
  - Now we need to offset each vertex such that the camera will be in the coordinate system origin. This is done to have better precision for points that are close to the camera, so unlike traditional systems where camera moves around, in this system camera always remain in the origin and objects moves around.
  - Next Perlin Noise is generated from vertex's world coordinates. For now there are 5 layers of noise with resolution and amplitude doubled for each layer, this ratio gives a fractal feeling to the result. The linear patterns probably appear due to random values not being random enough, this will be fixed in next state. And againg for minimizing precision errors noise vector are applied to vertices after these have been offset by camera position to avoid operations on values with significant difference in exponent. 
<br>![Could not find image](https://github.com/MrDebuggins/PlanetGen/assets/96007801/fba8d39d-f42e-43b0-9696-94fd28764145)<br>

# Final look
![perlin_variants](https://github.com/user-attachments/assets/b0390c16-342a-46b2-a577-3bfe2fe8fa3a)

