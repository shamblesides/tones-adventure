# Tone's Adventure
Tone's Adventure is an unfinished homebrew Gameboy Color game programmed in C
using the Gameboy Developer Kit (GBDK) library. I worked on it during late 2011
to early 2012. This was a very cool project to work on, and I hope to come back
to it someday!

[![Tone's Adventure demo video](https://i.imgur.com/WI4RpYo.png)](https://youtu.be/XFFF8lUgWpw "Hyeyo")


## Developing
No special IDE's were used in developing Tone's Adventure; just Vim and CMD.

Sprites and maps were created using
[Harry Mulder's Gameboy Development tools](http://www.devrs.com/gb/hmgd/intro.html).
Source .GBR and .GBM files can be found in this repository.


## Building
The current source has been tested on Windows using GBDK 2.95. Unfortunately,
I have so far been unable to build it using more modern Linux adaptations of
GBDK.

To make building on Windows as painless as possible, I have included working
build tool binaries in the `/vendor/` folder. To run the build, issue the
following command:

    make

To clean up built sources, run:

    make clean


## Running
Tone's Adventure has been tested on BGB 1.5.3, but other Gameboy emulators
will probably work too.


## Troubleshooting
*   Because of the old, arcane build tools used in this project, linking may
    fail with strange errors if the path to the project is too long. The
    threshold seems to be somewhere around 40 characters.
