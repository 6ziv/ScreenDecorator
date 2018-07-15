PREFIX := ver3
TARGET := ScreenDecorator

all:
	$(CXX) -D UNICODE -D _UNICODE -c $(PREFIX).cpp -o $(PREFIX).o
	$(CXX) -o $(TARGET).scr $(PREFIX).o -s -lcomctl32 -Wl,--subsystem,windows

.PHONY: clean
clean:
	-rm -f $(PREFIX).o