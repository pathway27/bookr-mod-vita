all:
	@(cd base; make; cp libbase.a ../libs)
	@(cd fonts; make; cp libfonts.a ../libs)
	@(cd mupdf; make; cp libmupdf.a ../libs)
	@(cd raster; make; cp libraster.a ../libs)
	@(cd stream; make; cp libstream.a ../libs)
	@(cd world; make; cp libworld.a ../libs)
#	@(cd apps; make; cp libapps.a ../libs)

clean:
	@(cd base; make clean)
	@(cd fonts; make clean)
	@(cd mupdf; make clean)
	@(cd raster; make clean)
	@(cd stream; make clean)
	@(cd world; make clean)
#	@(cd apps; make clean)
	@(cd libs; rm *.a)
