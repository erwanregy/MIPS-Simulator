MIPSSim:
	windres resources/icon.rc -O coff -o resources/icon.res
	g++ src/*.cpp -o MIPSSim resources/icon.res