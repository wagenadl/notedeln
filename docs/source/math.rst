Typesetting math and computer code
==================================

Typesetting mathematics
----------------------------------------------------------------------

When typing mathematical equations, having to frequently type the
backslash for special characters and [Ctrl][/] for italics can get
tiresome. To avoid this annoyance, press [Ctrl][‘] (that’s the key to
the left of the [1] on many qwerty keyboards) to enter (and exit)
“math” mode, which turns the |type| icon into |type-math|. (Math code can also
be entered by double-clicking the PIC icon or pressing [Shift][F2].)


.. |type-math| image:: type-math.png
              :height: 3ex
              :class: no-scaled-link

In math mode, special characters can be entered simply by typing their name and single-character words are typeset in italics.* To typeset a single-letter variable name in bold, bold italic, or roman, type its letter multiple times. Also in math mode, simple subscripts and superscripts can be typeset by just typing underscore or hat followed by the text of the sub- or superscript. As a result, an equation like “∫ 1∞1∕x2dx = 1” can be typeset simply by typing “int_1ˆinf 1/xˆ2 dx = 1”. A space, or punctuation followed by a space, ends to subscript or superscript.

Even double superscripts and subscripts are possible, to the degree that the second level is supported by unicode.* *At the moment, support is limited to digits, parentheses, and the plus, minus, and equal signs. For instance, “e-1 ∕ 2(x12+x22) ” can be typeset simply by typing “eˆ{-' 12(x_1ˆ2+x_2ˆ2)}”. (Note how the curly braces temporarily “protect” the inner expression.)


.. _codemode:

Typesetting computer code
--------------------------

When typing computer code, the automatic substitions in Figure 3.1,
3.2, and 3.3 can be a hindrance. To disable all automatic substitions,
press [Ctrl][F2] (or click the |type| icon with [Ctrl] held). The icon
will change to |type-code|, which signifies “Code” mode. Press [F2] to return
to normal text mode.

.. |type-code| image:: type-code.png
              :height: 3ex
              :class: no-scaled-link
