all:
	g++ main.cpp -o SP_work ./lib/libraylib.a ./lib/tinyfiledialogs.cpp -lX11

clean:
	rm SP_work
