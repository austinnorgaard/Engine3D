#include "olcPixelGameEngine.h"
#include "Engine3D.h"
#include <fstream>
#include <strstream>
#include <algorithm>

int main () {
	std::vector<std::string> objectFileNames = {"log.obj", "lamp.obj"};
	std::vector<std::string> logTextureFileNames = {"log.jpg", "lamp.png"};

	Engine3D demo (objectFileNames, logTextureFileNames);

	if (demo.Construct (512, 256, 2, 2)) {
		demo.Start ();
	}


	return EXIT_SUCCESS;
}