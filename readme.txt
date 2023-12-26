Requirements: gcc and a way to do primitive operations in terminal.

This is a repository with a primitive Monte Carlo simulation for straight muon paths within the JLAB Cosmic Watch setup.

The paths are chosen as such: The two detectors are parallel to one another. 
The software can accommodate two different configurations: 
    "Horizontal"    (0)     :   The two detectors are "flat"; the typical path across is along the short axis.
    "Vertical"      (1)     :   The two detectors are "tall"; the typical path across is along the long axis.

The input parameter "Separation" indicates the shortest distance between the *scintillators* - not between the detector casings.

The muon trajectories are sampled conditioning on hitting the bottom of the top detector.
Then, only the paths that go through the top of the bottom detector are accepted. The following parameters are stored:
    "x"     :   x position along bottom of the top detector in cm.
    "y"     :   y position along bottom of the top detector in cm.
    "theta" :   polar angle theta of the incoming particle in radians.
    "phi"   :   azimuthal angle of the incoming particle in radians.
    "l"     :   the length traversed in the top detector in cm.

It is made relatively easy to change the probability density function (pdf) of the incoming muons in terms
of the polar axis (theta), by altering the function "physical_pdf". Keep in mind that you also have to alter
the double "maximum" so that rejection sampling works. 

Copy compile.txt to terminal (with the name "test" changed), to compile your program. You need to have gcc installed.
You can then the program in terminal. There are 5 input parameters:
    "output_file" (str)     :   name of the output file that the program produces.
    "FinalIndex" (int)      :   the number of particles to be simulated.
    "mode" (int)            :   "0" for horizontal, "1" for vertical (see above).
    "separation" (double)   :   the separation between the *scintillators* (not casings) in cm.
    "seed" (int)            :   seed for drawing random numbers.