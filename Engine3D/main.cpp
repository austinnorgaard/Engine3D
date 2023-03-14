#include "olcPixelGameEngine.h"
#include "Engine3D.h"
#include <fstream>
#include <strstream>
#include <algorithm>

int main () {
	Engine3D demo;

	if (demo.Construct (512, 256, 2, 2)) {
		demo.Start ();
	}


	return EXIT_SUCCESS;
}