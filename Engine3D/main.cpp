#include "olcPixelGameEngine.h"
#include "Engine3D.h"
#include <fstream>
#include <strstream>
#include <algorithm>

int main () {
	Engine3D demo;

	if (demo.Construct(256, 240, 4, 4)) {
		demo.Start ();
	}


	return EXIT_SUCCESS;
}