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

#include "Options.h"
#include <QApplication>
#include <stdio.h>
#include <stdlib.h>
#include <QCommandLineParser>
#include <QCommandLineOption>

Options::Options(QApplication const &app) {
  QCommandLineOption cli_page("p", "Produce paginated output");
  QCommandLineOption cli_tiny("t", "Tiny [160px]");
  QCommandLineOption cli_small("s", "Small [320px]");
  QCommandLineOption cli_medium("m", "Medium [640px]");
  QCommandLineOption cli_large("l", "Large [1280px]");
  QCommandLineParser cli;
  cli.setApplicationDescription("\n"
    "Webgrab is a utility to download webpages and save archival copies\n"
    "in pdf format.\n"
    "\n"
    "Webgrab can also create a (png) thumbnail of the first page of the\n"
    "downloaded pdf.\n"
    "\n"
    "More information is at https://danielwagenaar.net/eln.");
  cli.addHelpOption();
  cli.addVersionOption();
  cli.addPositionalArgument("url", "Webpage to archive", "url");
  cli.addPositionalArgument("out.pdf", "Output filename", "out.pdf");
  cli.addPositionalArgument("out.png", "Thumbnail filename", "[out.png]");
  cli.addOption(cli_page);
  cli.addOption(cli_tiny);
  cli.addOption(cli_small);
  cli.addOption(cli_medium);
  cli.addOption(cli_large);
                             
  cli.process(app);

  if (cli.isSet("l"))
    imSize = 1280;
  else if (cli.isSet("m"))
    imSize = 640;
  else if (cli.isSet("s"))
    imSize = 320;
  else if (cli.isSet("t"))
    imSize = 160;
  else
    imSize = 640;

  paginate = cli.isSet("p");

  QStringList args = cli.positionalArguments();

  if (args.size() < 2) {
    cli.showHelp();
  }

  url = args.takeFirst();
  if (url.startsWith("/"))
    url = "file://" + url;
  else if (url.startsWith("http:"))
    ;
  else if (url.startsWith("https:"))
    ;
  else if (url.startsWith("file:"))
    ;
  else
    url = "http://" + url;

 
  out = args;

  qDebug() << "Hello world";
}

