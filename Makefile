surf2:surf2.cpp Makefile
	g++ -o surf2 surf2.cpp `pkg-config --cflags opencv` `pkg-config --libs opencv`

surfmovie:surfmovie.cpp Makefile
	g++ -o surfmovie surfmovie.cpp `pkg-config --cflags opencv` `pkg-config --libs opencv`

test:test.cpp Makefile
	g++ -o test test.cpp `pkg-config --cflags opencv` `pkg-config --libs opencv`

usbcam:usbcam.cpp Makefile
	g++ -o usbcam usbcam.cpp `pkg-config --cflags opencv` `pkg-config --libs opencv`

surftest:surf.cpp Makefile
	g++ -o surftest surf.cpp `pkg-config --cflags opencv` `pkg-config --libs opencv`

siftmovie:siftmovie.cpp Makefile
	g++ -o siftmovie siftmovie.cpp `pkg-config --cflags opencv` `pkg-config --libs opencv`

calibration:calibration.cpp Makefile
	g++ -o calibration calibration.cpp `pkg-config --cflags opencv` `pkg-config --libs opencv`

calibration_opencv2:calibration_opencv2.cpp Makefile
	g++ -o calibration_opencv2 calibration_opencv2.cpp `pkg-config --cflags opencv` `pkg-config --libs opencv`

8point:8point.cpp Makefile
	g++ -o 8point 8point.cpp `pkg-config --cflags opencv` `pkg-config --libs opencv`

match:match.cpp Makefile
	g++ -o match match.cpp `pkg-config --cflags opencv` `pkg-config --libs opencv`

catchim:catchim.cpp Makefile
	g++ -o catchim catchim.cpp `pkg-config --cflags opencv` `pkg-config --libs opencv`
