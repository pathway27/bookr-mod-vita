================================================================
Bookr - a document reader for the Sony PSP - http://bookr.sf.net
================================================================

About Bookr
===========
Bookr is a document reader for the Sony PSP licensed under the GNU General
Public License v2. At the moment it supports plain text and PDF files.


Install
=======
You need a Sony PSP with firmware version 1.5. Other firmware versions are not
supported. To install simply copy the bookr/ and bookr%/ folders to your Memory
Stick /PSP/GAME folder.


Acknowledgments
===============
Programmers - Carlos Carrasco Martinez and Edward Choh

MuPDF library - By Artifex Software, Inc. licensed under the AFPL license.
Visit: http://ghostpdf.com/new.html

ps2dev.org PSPSDK - http://ps2dev.org/psp/Projects


Found a bug?
============
Then please use the following link to report it:

http://sourceforge.net/tracker/?func=add&group_id=149290&atid=774195

If you found a file that crashes Bookr then it is of utmost importance that you
try to find a the same file online and paste a link to it in your bug report. We
cannot fix crashers without the offending files.


Want to request a new feature?
==============================
Then please use the following link to request it:

http://sourceforge.net/tracker/?func=add&group_id=149290&atid=774198

Please write a clear summary an description for your feature request. If you
have more than one feature to request please use more than one report. Bullet
lists inside a single feature request make it hard to track what we have
implemented.

You can also browse the existing feature requests to make sure you are not
filling a duplicate:

http://sourceforge.net/tracker/?group_id=149290&atid=774198


=========
Changelog
=========

Bookr 0.7.1 - xx xx 2006
============================
- Added support for PDF files with CJK fonts xxx EXPLAIN xxx
- Added a memory usage counter.
- Added single line scroll for the analog pad in the text viewer.
- Bookr will now try to block images from loading in PDF files when it is
runing low on memory.
- Fixed many memory leaks in the image loader for PDF files. Bookr should now
be able to load and display PDF files with more images than before.
- Fixed memory leaks releasing the PDF viewer singleton.
- Restored full page scroll with cursor buttons.
- Fixed font corruption for large font sizes in the text viewer.
- Fixed page positions in the text viewer after changing the font size and/or
font rotation. Boormarks for text files are not compatible with older
releases.

Bookr 0.7.0 - April 9th 2006
============================
- New toolbar interface, use the SELECT key to enable it.
- New bookmark system.
- Support for non-DRM PalmDoc/Mobipocket files (PDB/PRC extension.)
- Text-only support for basic HTML files.
- Screen rotation.
- New zoom options for PDF files.
- CPU and bus speed options.
- Option to disable page numbering labels.
- Option to invert PDF colors and to change out of bounds margin color.
- Added display of battery charge and clock to menu and toolbar.
- Fixed last folder viewing in the filechooser.
- Fixed centering in both PDF rendering modes.

Bookr 0.6.0
===========
- Added a bookmarks menu with support for multiple user bookmarks.
- Added a fast scroll mode for PDF files.
- Added selectable font, font size and colors for the plain text viewer.
Supports any TrueType font.
- Fixed centering of landscape PDF files. Only available in fast scroll mode.
- Minor UI enhancements, like scrollbars for long menus and error and warning
popups.

Bookr 0.5.2
===========
- Added bookmark for the last read page.
- Added error messages for corrupted PDF files.
- Fixed support for some protected PDF files.

Bookr 0.5.1
===========
- First public release.

psppdf 0.2
==========
- Public followup to 0.1 by Edward Choh.

psppdf 0.1
==========
- Proof-of-concept PDF viewer based on MuPDF. Privately released to a few
people.

