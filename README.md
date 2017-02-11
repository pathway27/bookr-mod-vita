This is the bookr v8.1 source from http://www.brewology.com/downloads/download.php?id=9542&mcid=1 (Accessed: 10-Feb-17).

# Bookr - a document reader for the Sony PSP - http://bookr.sf.net

## About Bookr

Bookr is a document reader for the Sony PSP licensed under the GNU General
Public License v2. At the moment it supports plain text, PDF, CHM, HTML, 
DJVU, PalmDoc documents.

Notes on usage of CHM:
- When you choose to open an CHM file, Bookr will try to extract the CHM content
to the cache folder (no need to extract later). The extracting job will be executing
in background if the CHM contains valid HHC file (table of content); otherwise
Bookr will try to extract all htm files first and open the index.htm file as
start page. In worst case, it will list all the htm files extracted. Then the
extracting thread will continue to extract all other files such as images, css...
So you may see pages with all images missing at first; but after the Memory Stick
LED light stop blinking (indicate 'extracting complete'), please refresh the page
to see the full content page.
- Bookr opens a Browser with 2 tabs: TOC and Content; please use SQUARE+L/RTRIGGER
to switch between them.

## Install
To install simply copy the Bookr folder to your Memory Stick \PSP\GAME folder. 

## Optional CJK PDF Support

In order to support Chinese, Japanese and Korean PDF documents an external font
pack must be downloaded and installed. Around 65MB of additional space is
required in your Memory Stick.

1) Download the file named cjk-fonts-1.zip from the following URL:
http://sourceforge.net/project/showfiles.php?group_id=149290&package_id=200753

2) Create the following folder in your Memory Stick, where X: is the drive unit
where your PSP is mounted:
X:\PSP\GAME\Bookr\fonts

3) Extract the contents of the cjk-fonts-1.zip file into the folder. The final
file structure must look like this:
X:\PSP\GAME\Bookr\fonts\Adobe-CNS1-UCS2
X:\PSP\GAME\Bookr\fonts\Adobe-GB1-UCS2
X:\PSP\GAME\Bookr\fonts\Adobe-Japan1-UCS2
X:\PSP\GAME\Bookr\fonts\Adobe-Korea1-UCS2
X:\PSP\GAME\Bookr\fonts\batang.ttf
X:\PSP\GAME\Bookr\fonts\bkai00mp.ttf
X:\PSP\GAME\Bookr\fonts\bsmi00lp.ttf
X:\PSP\GAME\Bookr\fonts\dotum.ttf
X:\PSP\GAME\Bookr\fonts\gbsn00lp.ttf
X:\PSP\GAME\Bookr\fonts\gkai00mp.ttf
X:\PSP\GAME\Bookr\fonts\kochi-gothic.ttf
X:\PSP\GAME\Bookr\fonts\kochi-mincho.ttf 

## Acknowledgments

Programmers 
  - Carlos Carrasco Martinez and Edward Choh (Original)
  - Christian Payeur (Control/GUI enhancements)
  - Yang.Hu (DJVU support)
  - Nguyen Chi Tam (CHM, HTML support)

MuPDF library - By Artifex Software, Inc. licensed under the AFPL license.
Visit: http://ghostpdf.com/new.html

ps2dev.org PSPSDK - http://ps2dev.org/psp/Projects 

chmlib 0.39 by Jed Wing <jedwin@ugcs.caltech.edu>

VnConv by Pham Kim Long <longp@cslab.felk.cvut.cz>

DjVuLibre 3.5 by Leon Bottou and Yann Le Cun

# Changelog

## Bookr V8.1 by Nguyen Chi Tam (nguyenchitam@gmail.com)
- Fixed crash when opening PDF file which has many pages.
- Enhance MuPDF, uses less memory.
- Merged with latest source code from CVS with following enhancements:
   + DJVU support by Yang.Hu (findreams at gmail dot com)
   + Control/GUI enhancements by Christian Payeur (christian dot payeur at gmail dot com)

## Bookr V8.0 by Nguyen Chi Tam (nguyenchitam@gmail.com)
- Uses internal Internet Browser to display CHM/HTML files.
- Can convert Vietnamese HTML content to display on Internet Browser (which
at the moment does not support Vietnamese Codepage)
- Uses swap button correctly
 
## Bookr 0.7.1 - August 16th 2006
- Added support for PDF files with Chinese, Japanese and Korean text.
- Added a memory usage counter.
- Added single line scroll for the analog pad in the text viewer.
- Bookr will now try to block images from loading in PDF files when it is
runing low on memory.
- Fixed many memory leaks in the image loader for PDF files. Bookr can load
and display PDF files with more images than before.
- Fixed memory leaks releasing the PDF viewer singleton.
- Restored full page scroll with cursor buttons.
- Fixed font corruption for large font sizes in the text viewer.
- Fixed page positions in the text viewer after changing the font size and/or
font rotation. Bookmarks for text files are not compatible with older
releases.

## Bookr 0.7.0 - April 9th 2006
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

## Bookr 0.6.0
- Added a bookmarks menu with support for multiple user bookmarks.
- Added a fast scroll mode for PDF files.
- Added selectable font, font size and colors for the plain text viewer.
Supports any TrueType font.
- Fixed centering of landscape PDF files. Only available in fast scroll mode.
- Minor UI enhancements, like scrollbars for long menus and error and warning
popups.

## Bookr 0.5.2
- Added bookmark for the last read page.
- Added error messages for corrupted PDF files.
- Fixed support for some protected PDF files.

## Bookr 0.5.1
- First public release.

## psppdf 0.2
- Public followup to 0.1 by Edward Choh.

## psppdf 0.1
- Proof-of-concept PDF viewer based on MuPDF. Privately released to a few
people. 
