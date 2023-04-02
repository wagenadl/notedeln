Position statement: Why use NotedELN?
=====================================

There are any number of software packages available that implement
electronic notebooks. So why should you choose NotedELN? NotedELN is
for you if:

- You want your notes to be stored in a human-readable format.

- You want your notes to be stored in a format that will be easy to
  parse electronically even 500 years from now.

- You want your notes to be protected against accidental deletion.

- You want your notes to be automatically dated.

- You want to concentrate on entering text and not on formatting.

- You want to be able to include images and simple graphics with your
  notes and you want that to be easy.

- You want your notebook software to be fast, even with thousands of
  pages of notes.

- You like your software to be open-source so that you can make your
  own improvements to it, and be confident that you can still run it
  20 years from now.

However, NotedELN may not be for you if:

- You want complete control over the formatting of your
  notes. (NotedELN does enable basic formatting.)

- You need to typeset complex equations in your notes. (NotedELN does
  have facilities for typesetting basic equations.)

- You need to typeset music in your notes.

- You need to import formatted documents into your notes. (NotedELN
  can archive web pages and pdf files for you, but they cannot be
  rendered onto the notebook pages.)

- You need a help desk on-call.

A note on development
---------------------

NotedELN is being developed by an active research
scientist. Practically, that means two things: On the positive side,
it means that I have a vested interest in fixing bugs and improving
NotedELN, because I use it daily. On the negative side, it means
that, by and large, new features are added only when I need them and
bugs are fixed when I have time. I certainly do welcome feature
requests, but I cannot guarantee that they will get implemented
quickly or at all. (If you are in a hurry, I will consider (paid)
consultancy related to NotedELN.) Finally, I definitely welcome
contributions to either the code or the documentation. I would be very
happy if NotedELN turned into a community-supported open source
project.

Features
--------

NotedELN notebooks consist of “entries” that fill one or more
pages. Each entry has a title and consists of paragraphs of text,
tables, and/or graphics canvases. Typesetting is deliberately simple:
you can create italics and bold face text as well as super- and
subscripts, but you cannot choose typefaces or font sizes (except as a
global option). These limits were a conscious design choice: the hope
is that this will force the user to concentrate on content rather than
form, just as you would in a paper notebook.

Support for graphics follows the same philosophy: you can
drag-and-drop or cut-and-paste images and (svg) vector graphics into a
notebook entry, and these graphics can be cropped and resized. You can
add simple symbols in a limited set of colors to the graphics as well
as draw freehand lines. It is also possible to attach text notes to
the graphics. You cannot, however, create arbitrarily complex graphics
in NotedELN; for that, the author recommends using the GIMP, Krita, or
Inkscape. It is easy to cut-and-paste from these programs into
NotedELN, and even easier to simply grab screenshots and paste them
into NotedELN.

NotedELN supports footnotes and references to other pages within the
same notebook, and automatically downloads and archives web pages if
you type their URL into a notebook entry. This, for instance,
facilitates keeping data sheets, MSDSs, and journal articles with your
notes.

A key feature of NotedELN is that each entry is stored in a separate
file. (A notebook is a folder on your hard disk with these files in a
subfolder.) This approach has numerous advantages:

- It makes for fast editing regardless of the size of the notebook;

- It limits the damage potential of hard disk corruption;

- It makes it convenient to use external version control software to
  archive your notebooks (`Git <http://git-scm.com>`_ is best supported);

- It facilitates electronically verifying when an entry was created; and

- It makes it much easier to manually correct broken files if somehow
  data does get compromised [#f1]_.

Another important design feature is that entries automatically get
locked (i.e., become immune to editing) after 24 hours [#f2]_. This
design choice might appear controversial, but it is an important
feature for a lab notebook: it encourages (in fact, enforces),
chronological note taking and preservation of history in experimental
records [#f3]_  [#f4]_.

NotedELN does not, at present, offer any facilities for multi-user
collaboration. However, if used in conjunction with version control
software, it is not hard to automatically maintain a central library
of many lab members’ notebooks. Lab members can then readily browse
each others’ notebooks. In addition, NotedELN can export anything from
an individual page to an entire notebook to pdf.

Contacting the author
---------------------

If you like NotedELN or find fault with it, if you discover a bug or
have a suggestion for a new feature, if you are interested in
improving this documentation or have a patch to contribute to the
code, I want to hear from you. My contact information is on the web at
http://www.danielwagenaar.net. I very much look forward to hearing
from you. I really do welcome questions, particularly if they help me
to improve NotedELN or its documentation.

Footnotes
---------

.. [#f1] Of course that’s not supposed to happen, but NotedELN, like
  all software, does have bugs, so it is good to know that failure is
  highly unlikely to be catastrophic.

.. [#f2] See :ref:`Editing old entries` in the user guide for a minor
         exception.

.. [#f4] You can write an annotation onto a previous entry, e.g., to
         point out a factual error, or to link to a follow-up notebook
         entry. Such annotations are typeset in a special color and
         automatically dated, so the historical record is clearly
         preserved.
         
.. [#f3] NotedELN on its own cannot be relied on to fully guarantee
  that entries aren’t modified post-hoc, because it is certainly
  possible to modify entries using an external text editor. However,
  judicious use of version control software can help document
  that such has not occurred.
