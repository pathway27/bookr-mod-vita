all:
	@(cd base; make; copy libbase.a ../libs)
	@(cd fonts; make; copy libfonts.a ../libs)
	@(cd mupdf; make; copy libmupdf.a ../libs)
	@(cd raster; make; copy libraster.a ../libs)
	@(cd stream; make; copy libstream.a ../libs)
	@(cd world; make; copy libworld.a ../libs)
#	@(cd apps; make; copy libapps.a ../libs)

clean:
	@(cd base; make clean)
	@(cd fonts; make clean)
	@(cd mupdf; make clean)
	@(cd raster; make clean)
	@(cd stream; make clean)
	@(cd world; make clean)
#	@(cd apps; make clean)
	@(cd libs; rm *.a)
