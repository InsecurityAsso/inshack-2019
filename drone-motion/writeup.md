# Drone motion

Once you have downloaded and extracted the archive, you find a (fake) log
recording drone sensors data.

You need to understand the log and in particular `accel` and `dir` abbreviations.

- `dir` stands for `direction` and is represented by a 3D vector.
- `accel` stands for `acceleration` here and is represented by a float.

You can see that the direction vector is normalized (all of its components have values in [-1,1]).

So all you need to do is read some articles about classical mechanics and implement a step by step
simulation using given input and plot the position you obtain for each step.

Looking at the drone trajectory, you can read the flag.

Note: see the `exploit/` folder for a working drone-trajectory-drawing tool.
