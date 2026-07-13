all:
	g++ genetic/GeneticAlgorithm.cpp data/RandomMatrixGenerator.cpp data/MatrixReader.cpp \
	gui.cpp -o SP_work ./lib/libraylib.a ./lib/tinyfiledialogs.cpp -lX11

clean:
	rm SP_work
