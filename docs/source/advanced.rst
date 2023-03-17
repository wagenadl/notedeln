Advanced topics
================


.. _vc:

Archiving (version control)
----------------------------

If you have Git installed on your computer, you can choose to have
your notebooks archived locally or to another computer using
Git. Simple enable the “Archiving” option and specify the place where
you want the archive to be stored.

Archiving locally is extremely easy, but of limited utility [#f1]_.
Archiving remotely from within NotedELN is slightly more involved. If
you have no experience with Git, it is probably best to remedy that
first. Some of the following is likely to be hard to understand
otherwise.

.. warning::
   Black diamond contents ahead!

For remote archiving, you need to have a host computer that you can
access by ssh without a password. Typically, that involves setting up
a public/private RSA key pair using ssh-keygen or similar and
appending the public key to the file “.ssh/authorized_keys” on the
server. Further details can be found elsewhere. In my experience,
doing this from a Windows computer is much trickier than from either
Linux or Mac OS; the most workable Windows solutions I have found are
“Pageant,” which is part of the “PuTTY” package [#f2]_, and the
unix-style ‘ssh-keygen,” “ssh-agent,” and “ssh-add” that come with
recent versions of Git SCM [#f3]_.

If you use Git to store your notebook on a remote host, you can also
access it from other computers. To do that, you would select “Clone
hosted notebook for local use” from the NotedELN opening
screen. Conveniently, once you have cloned the notebook, you can treat
it just like any other local notebook, with one caveat: you should not
open a notebook simultaneously on two computers, and always allow Git
to “commit and push” any changes back to the host [#f4]_.  Similar
cautions apply when you use solutions like Dropbox or iCloud for
holding your notebook.

.. warning::
   Double black diamond contents ahead!

It is also possible to turn an existing notebook into a Git
repository. There are two steps:

#. You should replace the line in your notebook’s “style.json” file
   that says “"vc": ""” to “"vc": "git"”;

#. You should locate the “.nb” folder, type git init to turn your
   notebook into a Git repository, then type some variant of ::

     ssh user@host git init --bare somewhere/nice/notebook.nb
     git push -u user@host:somewhere/nice/notebook.nb

   to set up the archive host.

Again, if this section doesn’t make sense to you, please first learn
about Git version control, then read it again before contacting
me. (And yes, I will be happy to assist.)

Customization
-------------

At present, you cannot graphically change the looks of a
notebook. However, inside each notebook folder, NotedELN creates a
file called “style.json” that defines many of the style parameters of
the notebook. I don’t have the time right now to document all of them
(feel free to contribute). Particularly important ones are
“page-width” and “page-height” which specify the width and height of a
notebook page in points (1/72”) and the various “…-font-family”
variables.

Footnotes
---------

.. [#f1] Unless “locally” actually means a network location that your
  computer maps to a path that merely “looks” local to NotedELN. Most
  operating systems are capable of that.
  
.. [#f2] At https://www.putty.org. See
  https://documentation.help/PuTTY/pageant.html for an introduction to
  Pageant.

.. [#f3] An excellent port of Git to Windows. At https://git-scm.com.

.. [#f4] If you fail to heed this warning, NotedELN will likely have
  to manually rebuild your index and table of contents. That’s not the
  end of the world. However, in rare instances, your notebook can get
  into a messy state from which recovery will require typing Git
  commands in a terminal window. Note that it is always completely
  safe to only use one client computer with Git at a time.
