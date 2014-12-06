
EXTRA_DIST = tutor.pdf tutor.cfg tutor.tex refman.pdf refman.cfg refman.tex

refman.pdf: refman.tex refman.cfg
	doxygen refman.cfg
	(cd latex; make; bibtex refman; make; make; dvips refman.dvi; ps2pdf refman.ps; mv refman.pdf ..)

tutor.pdf: tutor.tex tutor.cfg
	doxygen tutor.cfg
	(cd latex; make; bibtex refman; latex refman; latex refman; dvips refman; ps2pdf refman.ps; mv refman.pdf ../tutor.pdf)

clean:
	rm -rf html
	rm -rf latex
	rm -f refman.pdf tutor.pdf
