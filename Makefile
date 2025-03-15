# %@ name of the target
# $< first dependency for the target
# $^ all dependencies for the target

build: main.c main.h marc.c marc.h collections.c collections.h
	gcc main.c marc.c collections.c -o read_marc -Wall

clean:
	@rm -vf read_marc
