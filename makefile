default: o next.gb
clean:
	rmdir "o" /S /Q
	del *.gb
o:
	mkdir o
# MBC1, 8 rom banks, GBC support
next.gb: o/areas.o o/chunks.o o/next.o o/tone.o o/crab.o o/merm.o o/wizard.o o/scyfe.o o/bkg_tiles.o o/intro.o
	./vendor/gbdk/bin/lcc -Wl-yt1 -Wl-yo8 -Wl-yp0x143=0x80 -o next.gb o/areas.o o/chunks.o o/next.o o/tone.o o/crab.o o/merm.o o/wizard.o o/scyfe.o o/bkg_tiles.o o/intro.o
# Bank 0: main code (no -Wf-bo# argument)
o/areas.o: areas.c
	./vendor/gbdk/bin/lcc -c -o o/areas.o areas.c
o/chunks.o: chunks.c
	./vendor/gbdk/bin/lcc -c -o o/chunks.o chunks.c
o/next.o: next.c
	./vendor/gbdk/bin/lcc -c -o o/next.o next.c
# Bank 2: NPC tiles
o/tone.o: tiles/tone.c
	./vendor/gbdk/bin/lcc -Wf-bo2 -c -o o/tone.o tiles/tone.c
o/crab.o: tiles/crab.c
	./vendor/gbdk/bin/lcc -Wf-bo2 -c -o o/crab.o tiles/crab.c
o/merm.o: tiles/merm.c
	./vendor/gbdk/bin/lcc -Wf-bo2 -c -o o/merm.o tiles/merm.c
o/wizard.o: tiles/wizard.c
	./vendor/gbdk/bin/lcc -Wf-bo2 -c -o o/wizard.o tiles/wizard.c
o/scyfe.o: tiles/scyfe.c
	./vendor/gbdk/bin/lcc -Wf-bo2 -c -o o/scyfe.o tiles/scyfe.c
# Bank 3: Background tiles
o/bkg_tiles.o: tiles/bkg_tiles.c
	./vendor/gbdk/bin/lcc -Wf-bo3 -c -o o/bkg_tiles.o tiles/bkg_tiles.c
# Bank 4: Intro map
o/intro.o: maps/intro.c
	./vendor/gbdk/bin/lcc -Wf-bo4 -c -o o/intro.o maps/intro.c