
all: refman.pdf

refman.pdf:
	doxygen utility.cfg
	(cd latex; make; dvips refman.dvi; ps2pdf refman.ps; mv refman.pdf ..)

clean:
	rm -rf html
	rm -rf latex
	rm -f refman.pdf

EXTRA_DIST = utility.cfg utility.txt refman.pdf
