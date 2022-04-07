.PHONY: all cls build docs

all: cls build

cls:
	clear

build:
	gcc mrl.c -Og -g -ansi -o a.exe

docs:
	doxygen Doxyfile
# comment the next line if you want LaTeX
	rm -rf latex