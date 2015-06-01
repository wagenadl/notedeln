ALL: userguide.pdf eln.1 webgrab.1

userguide.pdf: userguide.tex
	pdflatex $<
	pdflatex $<

%.1: %.1.txt
	a2x --doctype manpage --format manpage $<
