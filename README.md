#L3Game

Just a little SDL/Chipmunk game made for a university project.

Need **libsdl1.2-dev**, **libsdl-gfx1.2-dev**, **libglib2.0-dev** and **libsdl-ttf2.0-dev**

The given libchipmunk.a is 64 bit.

##TODO :
  * rules 1, 2, 3
  * removal of the used letters
  * ~~video recording~~
  * valgrind
  * ??

##VIDEO_RECORDING :
 0. install **libav-tools**
 1. set VIDEO_RECORDING to 1
 2. create an img/ directory
 3. run the game :)
 4. in the img/ directory : ``` avconv -f image2 -i frame_%04d.bmp -r 60 foo.avi ```
