TARGET=recoverykeybyte

CC=gcc
OPT=-fno-stack-protector -O3

all: $(TARGET)
$(TARGET): main.c smallaes.c core.c core.h smallaes.h
	$(CC) $(OPT) -o $(TARGET) main.c smallaes.c core.c 

clean:
	rm -f $(TARGET)
	rm -f *.o 

