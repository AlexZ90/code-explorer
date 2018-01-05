all:
	$(CC) -fPIC --shared -o lib/libfoo.so src/foo.c -Iinclude $(CFLAGS)
	$(CC) -fPIC --shared -o lib/libbar.so src/bar.c -Iinclude $(CFLAGS)
	$(CXX)  -o test src/main.cpp src/test.cpp -Iinclude -Llib -lfoo -lbar -Wl,-rpath=lib $(CXXFLAGS)
