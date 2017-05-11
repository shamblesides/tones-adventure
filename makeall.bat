rem make object folder if it doesn't already exist
if not exist o mkdir o

rem put the music into bank 1
..\gbdk\bin\lcc -Wf-bo1 -c -o o\song1.o songs\song1.c

rem put the npc tiles into bank 2
..\gbdk\bin\lcc -Wf-bo2 -c -o o\tone.o tiles\tone.c
..\gbdk\bin\lcc -Wf-bo2 -c -o o\crab.o tiles\crab.c
..\gbdk\bin\lcc -Wf-bo2 -c -o o\merm.o tiles\merm.c
..\gbdk\bin\lcc -Wf-bo2 -c -o o\wizard.o tiles\wizard.c
..\gbdk\bin\lcc -Wf-bo2 -c -o o\scyfe.o tiles\scyfe.c

rem the bkg tiles go into bank 3
..\gbdk\bin\lcc -Wf-bo3 -c -o o\bkg_tiles.o tiles\bkg_tiles.c

rem the intro map goes to bank 4
..\gbdk\bin\lcc -Wf-bo4 -c -o o\intro.o maps\intro.c

rem do the rest of the mainstream making!
make