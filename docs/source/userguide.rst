Essential note taking
=====================

NotedELN has a deliberately sparse user interface intended to
encourage you to write notes without demanding too much of your
attention for technicalities and esthetical choices. It is the
author’s hope that the simplicity of the system will allow users to
come to experience note taking as a natural and unremarkable part of
their experimental work and data analysis rather than as a separate
chore.

Creating a new notebook
------------------------

When NotedELN starts, it displays a list of recent notebooks and also
offers the choice of opening a notebook that is not on the list or to
create a new notebook. When you click “Create new notebook,” it will
immediately ask you where you want to store that notebook. It will
then open the front page of your notebook, where you can change its
title and add your name as the author as well as your affiliation or
other relevant information. It is highly recommended that you give
your notebook a meaningful title, as that title will show up in
NotedELN’s opening screen in the future.

Other options on the opening screen are “Open other existing
notebook,” which speaks for itself, and “Clone hosted notebook for
local use,” which is explained under `Archiving (version control)`,
below.

To leave the front page and go to the first actual page of your new
notebook, press [Page Down] or [Ctrl][Shift][+] on your keyboard, or
use the navigation buttons in the tool bar.

Creating new entries
--------------------

To create a new entry, press [Ctrl][Shift][+] or click the |nav-plus| icon in
the tool bar. To encourage you to give your entries meaningful titles,
the cursor is positioned in the title field. The title you give to
your entry here is automatically copied to the table of contents.

.. |nav-plus| image:: nav-plus.png
              :height: 3ex
              :class: no-scaled-link                   

To move from the title to the first paragraph of your entry, simple
press [Enter] or [Tab].

Adding text
-----------

Text mode is NotedELN’s default mode, so you should be able to start
typing right away. If not, click the |type| mode icon (or press [F2])
to enter text mode. Click below existing contents or inside an
existing paragraph to start editing. Note that it is not possible to
edit an entry that is older than 24 hours.

.. |type| image:: type.png
              :height: 3ex
              :class: no-scaled-link                   


Text may be cut-and-pasted as you would expect using [Ctrl][X], [Ctrl][C], and [Ctrl][V] as in other programs, but NotedELN does not have an “undo” mechanism. To protect you against accidental deletion, you have to explicitly press [Delete] to delete a selection.

Adding graphics
---------------

Graphics can be added by dragging an image file onto the page or by
pressing [Ctrl][V] to paste an image from the system clipboard.

Various plot symbols as well as freehand lines can be added using the
|mark| and |squiggle| icons ([F4] and [F5]). Straight line segments
can be added by double clicking the |squiggle| icon (or by pressing
[Shift][F5]) which turns the squiggly line into a straight line:
|straight|. Several choices for symbol size, line width, and color are
available. (These options are not extendible. By limiting the options,
NotedELN hopes to encourage you to not spend too much time thinking
about the perfect color for your annotation.)

.. |mark| image:: mark.png
              :height: 3ex
              :class: no-scaled-link                   

.. |squiggle| image:: squiggle.png
              :height: 3ex
              :class: no-scaled-link                   

.. |straight| image:: straight.png
              :height: 3ex
              :class: no-scaled-link                   

Text annotations can be added to the graphics canvas using the |note|
icon ([F6]) and either clicking to place text or dragging to place
text with a connector line. Type faces and font sizes cannot be
changed (again, on purpose).

.. |note| image:: note.png
              :height: 3ex
              :class: no-scaled-link                   

Images, plot symbols, freehand and straight lines, and text
annotations can be moved around, cropped, and resized by selecting the
|move| icon (by clicking it or pressing [F3]). As a convenience, a
mouse drag with [Control] held performs the same manipulations without
selecting the |move| icon.

.. |move| image:: move.png
              :height: 3ex
              :class: no-scaled-link                   

Also with the |move| icon selected, you can change the wrap width of
text annotations by dragging the right edge of your annotation. The
end of a connector line can be moved by holding [Shift] while dragging
the annotation. (A note without a connector line can be given a
connector line by Shift-dragging; connector lines automatically vanish
if their ends are dragged into the text of the note.)

Graphics objects can be deleted with the |move| icon selected by
hovering the cursor over them and pressing [Delete]. They can be
restored by pressing [Insert]. An empty graphics canvas can be deleted
by pressing [Delete]. [Ctrl][Delete] works in any mode, provided there
is no active text cursor.

Adding tables
-------------

Tables can be inserted as their own paragraphs. Simply start typing
the contents of the first table cell, then hit [Tab] to create a
second cell. Navigation within a table is with [Tab] and [Shift][Tab]
for left and right, [Enter] and [Shift][Enter] for next and previous
line, and of course the arrow keys. New columns or rows can be
inserted by holding [Control] while navigating. Columns or rows can be
deleted by selecting the entire column or row and pressing
[Delete]. To make this easier, [Ctrl][A] cycles between selecting an
entire cell, an entire row, an entire column, and the entire table.

Adding links to external materials
-----------------------------------

Hyperlinks
^^^^^^^^^^

Press [Ctrl][L] to create a hyperlink. (If your hyperlink contains
spaces, you will have to select the text first, otherwise, NotedELN
figures out the boundaries of the link text automatically.) NotedELN
will automatically download the webpage and store an archival copy
with the notebook [#f1]_ [#f2]_.

Hovering over a link displays a thumbnail of the page, and
double clicking opens a pdf of the downloaded page. Double clicking
with [Shift] held opens the original web page. Hyperlinks are typeset
with a pale blue background once download is complete and with a pink
background if download fails. (A yellow background indicates that
download is in progress.)

Magic links
^^^^^^^^^^^^

NotedELN recognizes not just URL-style hyperlinks, but also a number
of other “magic” links:

- A small number (at most 4 digits), upon pressing [Ctrl][L] becomes a
  hyperlink to another page in the notebook.

- A large number (more than 4 digits) will be interpreted as a PubMed
  ID and will link to PubMed. When possible, the corresponding article
  will be automatically downloaded and archived with the notebook.

Do you have ideas for other kinds of magic links NotedELN should
support? Send me an email.

Navigation
------------

Navigation between pages and entries is done using [Page Up] and [Page
Down], using the scroll wheel of your mouse, or with the navigation
buttons overlaid on the bottom left of the notebook: |nav-prev| and
|nav-next| move up and down by one page; |nav-p10| and |nav-n10| move
by 10 pages. To go to the table of contents, press [Ctrl][Home] or
click |nav-toc|, and to go to the latest entry, press [Ctrl][End] or
click |nav-end|. Clicking on a page link (hold [Control] if the link is on
an editable page) activates the link. Press [Ctrl][Shift][+] (or click
the |nav-plus| icon) to start a new entry. (Pressing [Page Up] on an
untitled and empty entry abandons that entry.)

.. |nav-prev| image:: nav-prev.png
              :height: 3ex
              :class: no-scaled-link
                      
.. |nav-next| image:: nav-next.png
              :height: 3ex
              :class: no-scaled-link

.. |nav-p10| image:: nav-p10.png
              :height: 3ex
              :class: no-scaled-link                 

.. |nav-n10| image:: nav-n10.png
              :height: 3ex
              :class: no-scaled-link                 
                      
.. |nav-toc| image:: nav-toc.png
              :height: 3ex
              :class: no-scaled-link                 

.. |nav-end| image:: nav-end.png
              :height: 3ex
              :class: no-scaled-link                 


Saving your work
--------------------

You don’t have to! NotedELN automatically saves your work every 10
seconds and when you navigate to a different entry (if you have made
any changes). If you have configured `version control <vc>`, your
changes are automatically committed and pushed to the server every 5
minutes and when you close the notebook.


Editing old entries
-------------------

Cannot be done, by design. Just as you cannot change the outcome of
yesterday's experiments, yesterday's notes are fixed.

However, you can use the |note| icon ([F6]) to add so-called “late
notes” to a previous entry. Late notes are automatically set in a
distinct color and decorated with a date stamp. They may be
manipulated just like text annotations on a graphics canvas.

To indicate that an entry cannot be edited, NotedELN switches to
“browse” mode, indicated by the |browse| icon being automatically
selected.

.. |browse| image:: browse.png
              :height: 3ex
              :class: no-scaled-link

                      

Exporting and printing
---------------------------

NotedELN can export your entire notebook or portions of it to pdf or
print them directly. Simply press [Ctrl][P] or click the |nav-print|
icon to open the print dialog and select either “Print to pdf” or an
actual printer.

.. |nav-print| image:: nav-print.png
              :height: 3ex
              :class: no-scaled-link

Individual entries can also be exported as html by pressing
[Ctrl][Shift][S]. This feature is still slightly experimental. Styling
is not yet quite how I would like it to be. In the future, html output
may be integrated with the print dialog.


Searching your notebook
-------------------------------

NotedELN incorporates a simple but very useful full-text search
facility. Press [Ctrl][F] or click the |nav-find| icon to open the
search dialog, type any word or phrase, and press [Enter] or click
“OK.” A list with search results from the entire notebook will open;
click on a result to navigate to the relevant entry.

.. |nav-find| image:: nav-find.png
              :height: 3ex
              :class: no-scaled-link


Conclusion
----------

I hope that NotedELN will be useful to you, and that it will encourage
you to take more—and more usable—notes. I love to hear happy users’
stories. I also welcome bug reports of all kinds. And in the unlikely
event that NotedELN appears to have chewed up your notes, please do
not just throw away the broken notebook. Although I cannot offer any
guarantees (see the GPL license text!), it almost certainly can be
fixed. And I would be happy to try and help.

Footnote
----------

.. [#f1] If the link is a web page, the saved pdf is as it would
         be saved by the “Chromium” browser. Many websites generate
         pdf output that is somewhat different from how they appear in
         a browser. For a visually true snapshot of a website, taking
         a screenshot and pasting that into NotedELN is more accurate.

.. [#f2] If the link is not a web page but some other file type, the
         file is downloaded and archived, and NotedELN will attempt to
         create a thumbnail. Currently, this only works for pdf files.
