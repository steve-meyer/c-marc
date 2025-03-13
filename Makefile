# %@ name of the target
# $< first dependency for the target
# $^ all dependencies for the target

# PROGRAM = read_marc
# COMPILE_PROGRAM = gcc -Wall -v -o $@ $<
# COMPILE_OBJECT = gcc -Wall -v -c -o $@ $<

# all: $(PROGRAM)

# $(PROGRAM): main.c main.h marc.c marc.h collections.c collections.h
# 	$(COMPILE_PROGRAM) marc.o

# marc.o: marc.c marc.h collections.o
# 	$(COMPILE_OBJECT)
# 	echo "Finished marc.o"

# collections.o: collections.c collections.h
# 	$(COMPILE_OBJECT)
# 	echo "Finished collections.o"

build: main.c main.h marc.c marc.h collections.c collections.h
	gcc main.c marc.c collections.c -o read_marc -Wall

clean:
	@rm -vf read_marc
