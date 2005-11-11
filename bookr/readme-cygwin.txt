1. Introduction

Bookr runs on Linux, PSP (of course) and also on Windows using cygwin.
Here's some instructions on getting it to work on cygwin.

2. Making Bookr in cygwin

a. Install the cygwin environment (www.cygwin.com)
b. Make sure you install the requisite devel tools (gcc, make etc), 
   win32 and opengl devel files.
c. From this directory, do 'tar xvf cygwin-glut.tar.gz', the freeglut 
   header files will be untar to the 'cygwin' directory, and
   freeglut.dll in this directory. 
d. To make bookr, make -f Makefile.cyg and if all is well, bookr.exe
   will be created. (freeglut.dll is needed to run bookr.exe)

3. About freeglut & cygwin

Getting freeglut (freeglut.sourceforge.net) to work under cygwin is a 
little tricky. I couldn't compile freeglut under cygwin for some reason,
so I did the next best thing which was to compile freeglut using Visual
Studio. The great thing about gcc in cygwin is that it can use Win32 style
lib and dlls, so the aim is to use freeglut as a plain Win32 dll and link
that into bookr.

To compile freeglut:

a. Get the source package 
b. Compile it under Visual Studio, make sure that you create the dll 
   and link library (not the static library). 
c. So you should have freeglut.lib and freeglut.dll. Copy freeglut.lib
   to bookr/cygwin/freeglut_dyn.lib and copy freeglut.dll to bookr/
d. Copy the freeglut include files to bookr/cygwin
e. To make bookr, make -f Makefile.cyg and if all is well, bookr.exe
   will be created. (freeglut.dll is needed to run bookr.exe)

Any questions email me (Edward) at edwardchoh@sourceforge.net
