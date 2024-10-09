Formatting
==================

Inline with its mission, NotedELN only allows the most basic of
formatting options.

Standard typography
-------------------

*Italics*
    Press [Ctrl][I] or [Ctrl][/] to italicize the word under the cursor
    or the current selection (or to unitalicize).
  
**Bold face**
  
    Press [Ctrl][B] or [Ctrl][\*] (or [Ctrl][8]) for bold face.
  
_Underline_
  
    Press [Ctrl][U] or [Ctrl][_] (underscore; on my keyboard:
    [Ctrl][Shift][-]) for underline.
  
:sub:`Sub`\ scripts and :sup:`super`\ scripts
  
    Press [Ctrl][^] (or [Ctrl][6]) to create a superscript and
    [Ctrl][-] (minus) to create a subscript. Superscripts and
    subscripts can also be created by typing [^] or [_] followed by
    the text of the superscript or subscript and then pressing
    [Ctrl][.] (period).

.. _crossout:    

Editorializing: Highlighting and crossing out
---------------------------------------------

Any text, old or new, can be highlighted using the |highlight| icon
([F7]) or crossed out using the |strikeout| icon ([F8]). These
annotations can be removed using the |plain| icon ([F9]). Highlighting
of the selection or word under the cursor can also be toggled using
[Ctrl][!] (or [Ctrl][1]). Likewise, [Ctrl][=] toggles cross-out.


.. |highlight| image:: highlight.png
              :height: 3ex
              :class: no-scaled-link

.. |strikeout| image:: strikeout.png
              :height: 3ex
              :class: no-scaled-link
                      
.. |plain| image:: plain.png
              :height: 3ex
              :class: no-scaled-link


Footnotes
---------

Press [Ctrl][N] to create a footnote. Footnotes are connected to the
main text by arbitrary tags: the word at the cursor becomes the
tag. If you prefer to use symbols to tag footnotes, the symbols \*, †,
‡, §, ¶, and ♯ can be created by typing \*, +, ++, $, @, and # before
pressing [Ctrl][N]. Footnotes are deleted by deleting the tag in the
main text or by pressing [Ctrl][Shift][N] while the tag is
highlighted.

If your tag is a big integer, it is interpreted as a PubMed ID. In
that case, NotedELN will insert the corresponding citation in the note
for you automatically. (If you have suggestions for other kinds of
automatically created note contents, I want to hear from you.)


Indentation
-----------

By default, the first paragraph of a note is not indented, whereas
subsequent paragraphs are. To change the indentation of a paragraph,
press [Shift][Tab]. Repeat to cycle through the options:

- No indent
- Normal indent
- Reverse (“hanging”) indent

Typesetting quotations, computer code, and other imported materials
----------------------------------------------------------------------

Occasionally it is useful to typeset “imported” materials such as
quotations differently from the rest of your notes. In a small
concession to typographic nicety, NotedELN does this for you if you
press [Ctrl][Tab]. The paragraph will be typeset in a slightly
different color, a slightly smaller point size, and with slightly
larger margins. To revert, simply press [Ctrl][Tab] again.
