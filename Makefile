all:
	g++ main.cpp -o SP_work ./lib/libraylib.a -lX11

clean:
	rm SP_work
