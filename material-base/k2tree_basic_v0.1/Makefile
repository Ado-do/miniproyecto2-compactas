CPP=gcc

OBJECTS=basic.o bitrankw32int.o kTree.o
BINS=build_tree use_tree test_tree rebuild_tree invrebuild_tree revtest_tree rebuildCheck_tree
#CPPFLAGS=-Wall -g3 
CPPFLAGS=-Wall -O9 -g -DNDEBUG 
DEST=.

%.o: %.c
	$(CPP) $(CPPFLAGS) -c $< -o $@

all: bin

bin: $(OBJECTS) $(BINS)

build_tree:
	gcc $(CPPFLAGS) -o $(DEST)/build_tree buildk2tree.c $(OBJECTS) -lm

use_tree:
	gcc $(CPPFLAGS) -o $(DEST)/use_tree use.c $(OBJECTS)

test_tree:
	gcc $(CPPFLAGS) -o $(DEST)/test_tree testSpeed.c  $(OBJECTS)

revtest_tree:
	gcc $(CPPFLAGS) -o $(DEST)/revtest_tree testSpeedPred.c  $(OBJECTS)
	
rebuild_tree:
	gcc $(CPPFLAGS) -o $(DEST)/rebuild_tree rebuild.c  $(OBJECTS)

rebuildCheck_tree:
	gcc $(CPPFLAGS) -o $(DEST)/rebuildCheck_tree checkrebuild.c $(OBJECTS)


invrebuild_tree:
	gcc $(CPPFLAGS) -o $(DEST)/invrebuild_tree reverserebuild.c $(OBJECTS)
	
				
clean:
	rm -f $(OBJECTS) $(BINS)
	cd $(DEST); rm -f *.a $(BINS)

