// Options.C

#include "Options.H"
#include <QApplication>
#include <stdio.h>
#include <stdlib.h>

Options::Options(int &argc, char **&argv) {
  paginate = false;
  imSize = Medium;
  while (argc>1 && argv[1][0]=='-') {
    for (int n=1; argv[1][n]; n++) {
      switch (argv[1][n]) {
      case 'p': paginate=true; break;
      case 't': imSize = Tiny; break;
      case 's': imSize = Small; break;
      case 'm': imSize = Medium; break;
      case 'l': imSize = Large; break;
      case 'h': imSize = Huge; break;
      default: usage(); 
      }
    }
    argv++;
    argc--;
  }
  if (argc<2) {
    usage();
  }
  url = argv[1];
  if (url.startsWith("www."))
    url = "http://" + url;
  for (int n=2; n<argc; n++)
    out.append(argv[n]);
}

void Options::usage() {
  fprintf(stderr, "Usage: webgrab -p -tsmlh url outfile [outfile...]\n");
  exit(1);
}
