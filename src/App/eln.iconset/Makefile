all: eln_8x8.png eln_16x16.png eln_22x22.png eln_24x24.png eln_32x32.png eln_48x48.png	\
     eln_64x64.png eln_128x128.png eln_256x256.png eln_512x512.png

eln_%.png: eln_scalable.svg
	inkscape --export-width  $(shell echo $*|sed 's/x.*//') \
		 --export-height $(shell echo $*|sed 's/.*x//') \
		 --export-png $@ $<

# Shell fragment to convert scalable icon to bitmaps:
# in=eln_scalable.svg
# for r in 8 16 22 24 32 48 64 128 256 512 ; do
#     echo $r;
#     res=${r}x${r}
#     out=eln_${res}.png
#     # ImageMagick (convert) messes up the text
#     #convert -geometry ${res} ${in} ${out}
#     # Inkscape gets it right, of course
#     inkscape -w ${r} -h ${r} --export-filename=${out} ${in}
#     # librsvg-2.so.2 (rsvg-convert) messes up the text
#     #rsvg-convert --width ${r} --height ${r} --keep-aspect-ratio ${in} > ${out}
# done

.PHONY: all
