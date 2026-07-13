GENETIC_ALGORITMS_SRCS = genetic/GeneticAlgorithm.cpp data/RandomMatrixGenerator.cpp data/MatrixReader.cpp
UI_UTILS_SRCS = ui_utils/Button.cpp ui_utils/NumberBox.cpp ui_utils/Table.cpp

all:
	g++  $(GENETIC_ALGORITMS_SRCS) $(UI_UTILS_SRCS) gui_logic/GUI.cpp main.cpp -o SP_work \
 ./lib/libraylib.a ./lib/tinyfiledialogs.cpp -lX11

console_test:
	g++ $(GENETIC_ALGORITMS_SRCS)  genetic/test_ga.cpp -o ga_test

clean:
	rm -f SP_work ga_test
