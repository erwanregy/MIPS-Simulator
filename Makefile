MIPSSim:
	windres resources/icon.rc -O coff -o resources/icon.res
	g++ src/*.cpp src/*.hpp -o MIPSSim resources/icon.res