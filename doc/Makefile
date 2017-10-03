ALL: userguide.pdf eln.1 webgrab.1

userguide.pdf: ../doc/userguide.tex
	pdflatex $<
	pdflatex $<

%.1: ../doc/%.1.txt
	a2x --doctype manpage --format manpage --no-xmllint --destination-dir=../build-doc $<

clean:; rm -f userguide.log userguide.aux userguide.pdf
