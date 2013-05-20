// webgrab/Options.cpp - This file is part of eln

/* eln is free software: you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation, either version 3 of the License, or
   (at your option) any later version.

   eln is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with eln.  If not, see <http://www.gnu.org/licenses/>.
*/

// Options.C

#include "Options.H"
#include <QApplication>
#include <stdio.h>
#include <stdlib.h>

Options::Options(int &argc, char **&argv) {
  paginate = false;
  imSize = 640;
  while (argc>1 && argv[1][0]=='-') {
    for (int n=1; argv[1][n]; n++) {
      switch (argv[1][n]) {
      case 'p': paginate=true; break;
      case 't': imSize = 160; break;
      case 's': imSize = 320; break;
      case 'm': imSize = 640; break;
      case 'l': imSize = 1280; break;
      case 'h': imSize = 2560; break;
      default:
	if (n==1) {
	  bool ok;
	  imSize = QString(argv[1]+1).toInt(&ok);
	  if (!ok)
	    usage();
	  n = strlen(argv[1])-1; // awkward way to skip to end
	} else {
	  usage();
	}
	break;
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
