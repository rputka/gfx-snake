# Ryan Putka
# Makefile for lab 11

project: project.c
	gcc project.c gfx2.o -lX11 -lm -o project

clean:
	rm project
