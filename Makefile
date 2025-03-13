# %@ name of the target
# $< first dependency for the target
# $^ all dependencies for the target

PROGRAM = read_marc
COMPILE_PROGRAM = gcc -Wall -o $@ $<
COMPILE_OBJECT = gcc -Wall -c -o $@ $<

all: $(PROGRAM)

$(PROGRAM): main.c main.h marc.o collections.o
	$(COMPILE_PROGRAM) marc.o

marc.o: marc.c marc.h
	$(COMPILE_OBJECT)

collections.o: collections.c
	$(COMPILE_OBJECT)

clean:
	@rm -vf $(PROGRAM) marc.o collections.o
