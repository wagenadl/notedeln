

NotedELN

an Electronic Lab Notebook

By Daniel A. Wagenaar

version 1.4.99

Copyright (c) 2013–2021

 

Copyright (C) 2013–2021 Daniel A. Wagenaar

NotedELN is free software: you can redistribute it and/or modify it under the terms of the GNU General Public License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.

This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.

You should have received a copy of the GNU General Public License along with this program. If not, see http://www.gnu.org/licenses.
Contents
1 Introduction
 1.1 Why use NotedELN?
 1.2 Features
 1.3 Contacting the author
2 Installation
 2.1 Installing precompiled binaries
 2.2 Compiling the source
3 Using NotedELN
 3.1 Some general notes
 3.2 Creating a new notebook
 3.3 Creating new entries
 3.4 Adding text
 3.5 Adding graphics
 3.6 Adding tables
 3.7 Saving your work
 3.8 Navigation
 3.9 Editing old entries
 3.10 Formatting
 3.11 Special characters
 3.12 Footnotes
 3.13 Hyperlinks
 3.14 Magic links
 3.15 Typesetting quotations, computer code, and other imported materials
 3.16 Typesetting mathematics
 3.17 Typesetting computer code
 3.18 Exporting and printing
 3.19 Searching your notebook
 3.20 Customization
 3.21 Archiving (version control)
 3.22 Conclusion
Chapter 1
Introduction

This document describes the installation and usage of NotedELN, an electronic lab notebook written by Daniel Wagenaar. This introduction will not cover why you should keep a lab notebook, nor why an electronic lab notebook may be desirable. You already know that. It will however, cover some of the ideas behind this particular implementation.
1.1 Why use NotedELN?

There are any number of software packages available that implement electronic notebooks. So why should you choose NotedELN? NotedELN is for you if:

    You want your notes to be stored in a human-readable format.

    You want your notes to be stored in a format that will be easy to parse electronically even 500 years from now.

    You want your notes to be protected against accidental deletion.

    You want your notes to be automatically dated.

    You want to concentrate on entering text and not on formatting.

    You want to be able to include images and simple graphics with your notes and you want that to be easy.

    You want your notebook software to be fast, even with thousands of pages of notes.

    You like your software to be open-source so that you can make your own improvements to it, and be confident that you can still run it 20 years from now.

However, NotedELN may not be for you if:

    You want complete control over the formatting of your notes. (NotedELN will allow you some coarse control.)

    You need to typeset complex equations in your notes. (NotedELNwill allow you to set basic equations.)

    You need to typeset music in your notes.

    You need to import formatted documents into your notes. (Noted ELN can archive web pages and pdf files for you, but they cannot be rendered onto the notebook pages.)

    You need a fully polished graphical user interface.

    You need a help desk on-call.

Lastly, a note on development. NotedELN is being developed by an active research scientist. Practically, that means two things: On the positive side, it means that I have a vested interest in fixing bugs and improving NotedELN, because I use it daily. On the negative side, that means that, by and large, new features are added only when I need them and bugs are fixed when I have time. I certainly do welcome feature requests, but I cannot guarantee that they will get implemented quickly or at all. (If you are in a hurry, I will consider (paid) consultancy related to NotedELN.) Finally, I definitely welcome contributions to either the code or the documentation. I would be very happy if NotedELN turned into a community-supported open source project.

1.2 Features

NotedELN notebooks consist of “entries” that fill one or more pages. Each entry has a title and consists of paragraphs of text, tables, and/or graphics canvases. Typesetting is deliberately simple: you can create italics and bold face text as well as super- and subscripts, but you cannot choose typefaces or font sizes (except as a global option). These limits were a conscious design choice: the hope is that this will force the user to concentrate on content rather than form, just as you would in a paper notebook.

Graphic manipulation is similarly rudimentary: you can drag-and-drop or cut-and-paste images and (svg) vector graphics into a notebook entry, and these graphics can be cropped and resized, but they cannot, e.g., be rotated or recolored. You can add simple symbols in a limited set of colors to the graphics as well as draw freehand lines. It is also possible to attach text notes to the graphics. You cannot, however, create arbitrarily complex graphics in NotedELN; for that, the author recommends using the GIMP* or Inkscape* * http://www.inkscape.org. . It is easy to cut-and-paste from these programs into NotedELN, and even easier to simply grab screenshots and paste them into NotedELN.

NotedELN supports footnotes and references to other pages within the same notebook, and automatically downloads and archives web pages if you type their URL into a notebook entry. This, for instance, facilitates keeping data sheets, MSDSs, and journal articles with your notes.

A key feature of NotedELN is that each entry is stored in a separate file. (A notebook is a folder on your hard disk with these files in a subfolder.) This approach has numerous advantages:

    It makes for fast editing regardless of the size of the notebook;

    It limits the damage potential of hard disk corruption;

    It makes it convenient to use external version control software to archive your notebooks (Git† is best supported);

    It facilitates electronically verifying when an entry was created; and

    It makes it much easier to manually correct broken files if somehow data does get compromised.* 

Another important design feature is that entries automatically get locked (i.e., become immune to editing) after 24 hours.† †See below under “Editing old entries” for a minor exception. This design choice might be controversial, but it is an important feature for a lab notebook: it encourages (in fact, enforces), chronological note taking and discourages manipulating data post-hoc.* *NotedELN on its own cannot be relied on to fully guarantee that entries aren’t modified post-hoc, because it is certainly possible to modify entries using an external text editor. However, judicious use of version control software can be used to document that such abuse has not occurred.

NotedELN does not, at present, offer any facilities for multi-user collaboration. However, if used in conjunction with version control software, it is not hard to automatically maintain a central library of many lab members’ notebooks. Lab members can then readily browse each others’ notebooks. In addition, NotedELN can export anything from an individual page to an entire notebook to pdf.
1.3 Contacting the author

If you like NotedELN or find fault with it, if you discover a bug or have a suggestion for a new feature, if you are interested in improving this documentation or have a patch to contribute to the code, I want to hear from you. My contact information is on the web at http://www.danielwagenaar.net. I very much look forward to hearing from you. I realize that this guide is extremely terse, and I really do welcome questions, particularly if they help me to improve NotedELN or its documentation.
Chapter 2
Installation

The latest version of the software can always be downloaded from either of the following places:

    https://github.com/wagenadl/eln/releases

    http://www.danielwagenaar.net/eln

2.1 Installing precompiled binaries

To install on Windows 10, using the “.exe” installation package. To install on Mac OS X, opening the “.dmg” archive and place “NotedELN.app” anywhere on your system. Installation on Debian, Ubuntu, or Mint Linux should be easy using the “.deb” installation package. At present, installation on other flavors of Linux will require compiling the sources yourself, but this should be straightforward (see below).

At present, neither Android devices nor iPads are currently supported, simply because the author doesn’t own any. If you are interested in porting NotedELN to either of these platforms or would like to commission me to do so, please contact me by email.

Please note that development occurs primarily on Linux, so the Windows and Mac OS versions may lag behind.

2.2 Compiling the source

To compile the source, download the source at https://github.com/wagenadl/eln. You will need “Qt 5” version 5.6 or later.

You will need a C++ compiler, the Qt development packages and “cmake”. On Ubuntu Linux, this is as simple as sudo apt-get install g++ cmake qtmultimedia5-dev  libqt5webkit5-dev libqt5svg5-dev. On Mac OS, you need the “Command Line tools for XCode” from the Apple Developers’ web site* . On Windows you will need Visual Studio (Community Edition).

Open a terminal (git bash works great in Windows), then “cd” to the root of the unpacked source archive. In the terminal, type mkdir build; cd build.

On Linux and Mac, you can then simply type cmake ..; cmake --build . (notice the literal period at the end) and fetch a cup of tea. Then, either manually copy the files “notedeln” and “webgrab” to a location on your PATH, or type sudo make install to install into “/usr/local/bin”. On Linux, you can also type cpack to create a “.deb” package. Likewise, on Mac, cpack creates a dmg package that can easily be carried to other machines.

On Windows, “cmake” cannot guess the location of Qt. You will need something like CMAKE_PREFIX_PATH=/c/Qt/5.15.2/msvc2019_64/lib/cmake/Qt5 cmake .. (notice the literal periods at the end). The path to Qt may need to be changed. Follow this by cmake --build . --config Release and fetch your tea. At that point, you can run NotedELN in place (in the Release subdirectory), or you can create an installation package using cpack and install that.
Chapter 3
Using NotedELN

NotedELN has a deliberately sparse user interface that may take a little getting used to. It is the author’s hope, however, that users will quickly get to appreciate the simplicity of the system.
3.1 Some general notes

NotedELN is intended to be fully usable either with traditional laptop or desktop computers or with tablets. To facilitate that, it only uses the left mouse button. NotedELN can also be operated almost exclusively from the keyboard.

3.2 Creating a new notebook

When NotedELN starts, it displays a list of recent notebooks and also offers the choice of opening a notebook that is not on the list or to create a new notebook. When you click “Create new notebook,” it will immediately ask you where you want to store that notebook. It will then open the front page of your notebook, where you can change its title and add your name as the author as well as your affiliation or other relevant information. It is highly recommended that you give your notebook a meaningful title, as that title will show up in NotedELN’s opening screen in the future.

Other options on the opening screen are “Open other existing notebook,” which speaks for itself, and “Clone hosted notebook for local use,” which is explained under “Archiving (version control),” below.

To leave the front page and go to the first actual page of your new notebook, press [Page Down] or [Ctrl][Shift][+] on your keyboard, or use the navigation buttons in the tool bar.

3.3 Creating new entries

To create a new entry, press [Ctrl][Shift][+] or click the PIC icon in the tool bar. To encourage you to give your entries meaningful titles, the cursor is positioned in the title field. The title you give to your entry here is automatically copied to the table of contents.

To move from the title to the first paragraph of your entry, simple press [Enter] or [Tab].

3.4 Adding text

Text mode is NotedELN’s default mode, so you should be able to start typing right away. If not, click the PIC mode icon (or press [F2]) to enter text mode. Click below existing contents or inside an existing paragraph to start editing. Note that it is not possible to edit an entry that is older than 24 hours.

Text may be cut-and-pasted as you would expect using [Ctrl][X], [Ctrl][C], and [Ctrl][V] as in other programs, but NotedELN does not have an “undo” mechanism. To protect you against accidental deletion, you have to explicitly press [Delete] to delete a selection.

3.5 Adding graphics

Graphics can be added by dragging an image file onto the page or by pressing [Ctrl][V] to paste an image from the system clipboard.

Various plot symbols as well as freehand lines can be added using the PIC and PIC icons ([F4] and [F5]). Straight line segments can be added by double clicking the PIC icon (or by pressing [Shift][F5]) which turns the squiggly line into a straight line: PIC. Several choices for symbol size, line width, and color are available. (These options are not extendible. By limiting the options, NotedELN hopes to encourage you to not spend too much time thinking about the perfect color for your annotation.)

Text annotations can be added to the graphics canvas using the PIC icon ([F6]) and either clicking to place text or dragging to place text with a connector line. Type faces and font sizes cannot be changed (again, on purpose).

Images, plot symbols, freehand and straight lines, and text annotations can be moved around, cropped, and resized by selecting the PIC icon (by clicking it or pressing [F3]). As a convenience, a mouse drag with [Control] held performs the same manipulations without selecting the PIC icon.

Also with the PIC icon selected, you can change the wrap width of text annotations by dragging the right edge of your annotation. The end of a connector line can be moved by holding [Shift] while dragging the annotation. (A note without a connector line can be given a connector line by Shift-dragging; connector lines automatically vanish if their ends are dragged into the text of the note.)

Graphics objects can be deleted with the PIC icon selected by hovering the cursor over them and pressing [Delete]. They can be restored by pressing [Insert]. An empty graphics canvas can be deleted by pressing [Delete]. [Ctrl][Delete] works in any mode, provided there is no active text cursor.

3.6 Adding tables

Tables can be inserted as their own paragraphs. Simply start typing the contents of the first table cell, then hit [Tab] to create a second cell. Navigation within a table is with [Tab] and [Shift][Tab] for left and right, [Enter] and [Shift][Enter] for next and previous line, and of course the arrow keys. New columns or rows can be inserted by holding [Control] while navigating. Columns or rows can be deleted by selecting the entire column or row and pressing [Delete]. To make this easier, [Ctrl][A] cycles between selecting an entire cell, an entire row, an entire column, and the entire table.

3.7 Saving your work

You don’t have to! NotedELN automatically saves your work every 10 seconds (if you have made any changes) and when you navigate to a different entry (ditto). If you have configured version control (see below), your changes are automatically committed and pushed to the server every 5 minutes and when you close the notebook.

3.8 Navigation

Navigation between pages and entries is done using [Page Up] and [Page Down], using the scroll wheel of your mouse, or with the navigation buttons overlaid on the bottom left of the notebook: PIC and PIC move up and down by one page; PIC and PIC move by 10 pages. To go to the table of contents, press [Ctrl][Home] or click PIC, and to go to the latest entry, press [Ctrl][End] or click PIC. Clicking on a page link (hold [Control] if the link is on an editable page) activates the link. Press [Ctrl][Shift][+] (or click the PIC icon) to start a new entry. (Pressing [Page Up] on an untitled and empty entry abandons that entry.)

3.9 Editing old entries

Cannot be done. Except that you can use the PIC icon ([F6]) to create so-called “late notes.” These are automatically set in a distinct color and decorated with a date stamp. They may be manipulated just like text annotations on a graphics canvas. To indicate that an entry cannot be edited, NotedELN switches to “browse” mode, indicated by the PIC icon being automatically selected.

3.10 Formatting

NotedELN doesn’t offer advanced formatting, but it does offer some basic options: Press [Ctrl][I] or [Ctrl][/] to italicize the word under the cursor or the current selection (or to unitalicize). Press [Ctrl][B] or [Ctrl][*] (or [Ctrl][8]) for bold face. Press [Ctrl][U] or [Ctrl][_] (underscore; on my keyboard: [Ctrl][Shift][-]) for underline. Type [Ctrl][ˆ] (or [Ctrl][6]) to create a superscript and [Ctrl][-] (minus) to create a subscript.*

In addition, any text, old or new, can be highlighted using the PIC icon ([F7]) or crossed out using the PIC icon ([F8]). These annotations can be removed using the PIC icon ([F9]). Highlighting of the selection or word under the cursor can also be toggled using [Ctrl][!] (or [Ctrl][1]). Likewise, [Ctrl][=] toggles cross-out.
3.11 Special characters

NotedELN supports most of unicode and—presumably—you can use any input method supported by Qt to enter text.* *I have only tested this with the “compose” key method in “Gnome”; I am interested in your test results. In addition, the following substitutions are made automatically as you type (Figure 3.1). (See section 3.17 for how to prevent these substitutions.)

  PIC  
Figure 3.1: Automatic character substitutions. To get the glyph on the left, type the character sequence on the right. a. General punctuation. b. Arrows (see also Figure 3.2i). c. Less than and greater than. d. Decorated equals signs. e. Other mathematical operators.

In addition to the automatic substitutions, there are many symbols that can be obtained by typing a backslash followed by their name (Figure 3.2). Extending this list is easy, so let me know if you have suggestions.

As an alternative to standard unicode input methods for entering accented letters, NotedELN supports creating a select group of accented letters by typing a backslash followed by a symbol and a letter (Figure 3.3), as in “Se' ˜nor” for “Señor” or “gar' ,con” for “garçon”.

PIC
Figure 3.2: Symbols that may be obtained by a TeX-like command. To get the symbols on the left, type a backslash followed by the character sequence on the right, then keep typing. a, b. Lowercase and uppercase Greek letters. c. Fractions. d. Scientific units. e. Mathematical operators. f. Other mathematical symbols. g. Other symbols. h. General punctuation. i. Arrows. j. Mathematical accents. (The codes in j are different from the others, in that the accent is placed over the preceding character rather than as a separate entity.)

  PIC  
Figure 3.3: Accented letters that may be obtained by a TeX-like sequence. To get the accented letters on the left, type a backslash followed by the character sequence on the right, then keep typing.

3.12 Footnotes

Press [Ctrl][N] to create a footnote. Footnotes are connected to the main text by arbitrary tags: the word at the cursor becomes the tag. If you prefer to use symbols to tag footnotes, the symbols *, †, ‡, §, ¶, and ♯ can be created by typing *, +, ++, $, @, and # before pressing [Ctrl][N]. Footnotes are deleted by deleting the tag in the main text or by pressing [Ctrl][Shift][N] while the tag is highlighted.

If your tag is a big integer, it is interpreted as a PubMed ID. In that case, NotedELN will insert the corresponding citation in the note for you automatically. (If you have suggestions for other kinds of automatically created note contents, I want to hear from you.)

3.13 Hyperlinks

Press [Ctrl][L] to create a hyperlink. (If your hyperlink contains spaces, you will have to select the text first, otherwise, NotedELN figures out the boundaries of the link text automatically.)

Hovering over a link displays a thumbnail of the page* *Currently, the Mac and Windows versions merely show the title of a web page while hovering. I hope to restore thumbnailing when Qt’s QWebEngine technology further matures. , and double clicking opens a pdf of the downloaded page. Double clicking with [Shift] held opens the original web page. Hyperlinks are typeset with a pale blue background once download is complete and with a pink background if download fails. (A yellow background indicates that download is in progress.)

3.14 Magic links

NotedELN recognizes not just URL-style hyperlinks, but also a number of other “magic” links:

    A small number (at most 4 digits), upon pressing [Ctrl][L] becomes a hyperlink to another page in the notebook.

    A large number (more than 4 digits) will be interpreted as a PubMed ID and will link to PubMed. When possible, the corresponding article will be automatically downloaded and archived with the notebook.

3.15 Typesetting quotations, computer code, and other imported materials

Occasionally it is useful to typeset “imported” materials such as quotations differently from the rest of your notes. In a small concession to typographic nicety, NotedELN does this for you if you press [Ctrl][Tab]. The paragraph will be typeset in a slightly different color, a slightly smaller point size, and with slightly larger margins. To undo, simply press [Ctrl][Tab] again. Similarly, indentation can be cycled between indented paragraphs (the default), non-indented paragraphs, and “dedented” paragraphs, which is useful for typing bullet lists. This is done by pressing [Shift][Tab].

3.16 Typesetting mathematics

When typing mathematical equations, having to frequently type the backslash for special characters and [Ctrl][/] for italics can get tiresome. To avoid this annoyance, press [Ctrl][‘] (that’s the key to the left of the [1] on many qwerty keyboards) to enter (and exit) “math” mode, which turns the PIC icon into PIC. (Math code can also be entered by double-clicking the PIC icon or pressing [Shift][F2].)

In math mode, special characters can be entered simply by typing their name and single-character words are typeset in italics.* To typeset a single-letter variable name in bold, bold italic, or roman, type its letter multiple times. Also in math mode, simple subscripts and superscripts can be typeset by just typing underscore or hat followed by the text of the sub- or superscript. As a result, an equation like “∫ 1∞1∕x2dx = 1” can be typeset simply by typing “int_1ˆinf 1/xˆ2 dx = 1”. A space, or punctuation followed by a space, ends to subscript or superscript.

Even double superscripts and subscripts are possible, to the degree that the second level is supported by unicode.* *At the moment, support is limited to digits, parentheses, and the plus, minus, and equal signs. For instance, “e-1 ∕ 2(x12+x22) ” can be typeset simply by typing “eˆ{-' 12(x_1ˆ2+x_2ˆ2)}”. (Note how the curly braces temporarily “protect” the inner expression.)
3.17 Typesetting computer code

When typing computer code, the automatic substitions in Figure 3.1, 3.2, and 3.3 can be a hindrance. To disable all automatic substitions, press [Ctrl][F2] (or click the PIC icon with [Ctrl] held). The icon will change to PIC, which signifies “Code” mode. Press [F2] to return to normal text mode.

3.18 Exporting and printing

NotedELN can export your entire notebook or portions of it to pdf or print them directly. Simply press [Ctrl][P] or click the PIC icon to open the print dialog and select either “Print to pdf” or an actual printer.

Individual entries can also be exported as html by pressing [Ctrl][Shift][S]. This feature is still slightly experimental. Styling is not yet quite how I would like it to be. In the future, html output may be integrated with the print dialog.

3.19 Searching your notebook

NotedELN incorporates a simple but very useful full-text search facility. Press [Ctrl][F] or click the PIC icon to open the search dialog, type any word or phrase, and press [Enter] or click “OK.” A list with search results from the entire notebook will open; click on a result to navigate to the relevant entry.

3.20 Customization

At present, you cannot graphically change the looks of a notebook. However, inside each notebook folder, NotedELN creates a file called “style.json” that defines many of the style parameters of the notebook. I don’t have the time right now to document all of them (feel free to contribute). Particularly important ones are “page-width” and “page-height” which specify the width and height of a notebook page in points (1/72”) and the various “…-font-family” variables.

3.21 Archiving (version control)

If you have Git installed on your computer, you can choose to have your notebooks archived locally or to another computer using Git. Simple enable the “Archiving” option and specify the place where you want the archive to be stored.

Archiving locally is extremely easy, but of limited utility.* *Unless “locally” actually means a network location that your computer maps to a path that merely “looks” local to NotedELN. Most operating systems are capable of that. Archiving remotely from within NotedELN is slightly more involved. If you have no experience with Git, it is probably best to remedy that first. Some of the following is likely to be hard to understand otherwise.

Caution: Black diamond contents ahead. For remote archiving, you need to have a host computer that you can access by ssh without a password. Typically, that involves setting up a public/private RSA key pair using ssh-keygen or similar and appending the public key to the file “.ssh/authorized_keys” on the server. Further details can be found elsewhere. In my experience, doing this from a Windows computer is much trickier than from either Linux or Mac OS; the most workable Windows solutions I have found are “Pageant,” which is part of the “PuTTY” package,† †At https://www.putty.org. See https://documentation.help/PuTTY/pageant.html for an introduction to Pageant. and the unix-style ‘ssh-keygen,” “ssh-agent,” and “ssh-add” that come with recent versions of Git SCM.*

If you use Git to store your notebook on a remote host, you can also access it from other computers. To do that, you would select “Clone hosted notebook for local use” from the NotedELN opening screen. Conveniently, once you have cloned the notebook, you can treat it just like any other local notebook, with one caveat: you should not open a notebook simultaneously on two computers, and always allow Git to “commit and push” any changes back to the host.* *If you fail to heed this warning, NotedELN will likely have to manually rebuild your index and table of contents. That’s not the end of the world. However, in rare instances, your notebook can get into a messy state from which recovery will require typing Git commands in a terminal window. Note that it is always completely safe to only use one client computer with Git at a time. Similar cautions apply when you use solutions like Dropbox or iCloud for holding your notebook.

Warning: Double black diamond contents ahead. It is also possible to turn an existing notebook into a Git repository. There are two steps:

    you should replace the line in your notebook’s “style.json” file that says “"vc": ""” to “"vc": "git"”;

    you should locate the “.nb” folder, type git init to turn your notebook into a Git repository, then type some variant of

    ssh user@host git init --bare somewhere/nice/notebook.nb

    git push -u user@host:somewhere/nice/notebook.nb

    to set up the archive host.

Again, if this section doesn’t make sense to you, please first learn about Git version control, then read it again before contacting me. (And yes, I will be happy to assist.)
3.22 Conclusion

I hope that NotedELN will be useful to you, and that it will encourage you to take more—and more usable—notes. I love to hear happy users’ stories. I also welcome bug reports of all kinds. And in the unlikely event that NotedELN appears to have chewed up your notes, please do not just throw away the broken notebook. Although I cannot make any guarantees (see the GPL license text!), it almost certainly can be fixed. And I would be happy to try and help.

Pasadena, May 2013;

Cincinnati, February 2014;

Woods Hole, June 2015;

Pasadena, January 2016, January 2017, April 2019;

Altadena, November 2020, May 2021. 
