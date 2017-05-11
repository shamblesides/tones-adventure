rem main code stuff goes in bank 0 (no -Wf-boX argument!!)
..\gbdk\bin\lcc -c -o o\areas.o areas.c
..\gbdk\bin\lcc -c -o o\chunks.o chunks.c
..\gbdk\bin\lcc -c -o o\next.o next.c

rem MBC1, 8 rom banks, GBC support
..\gbdk\bin\lcc -Wl-yt1 -Wl-yo8 -Wl-yp0x143=0x80 -o next.gb o\areas.o o\chunks.o o\next.o o\tone.o o\crab.o o\merm.o o\wizard.o o\scyfe.o o\bkg_tiles.o o\intro.o

..\bgb\bgb next.gb