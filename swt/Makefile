PROGRAM = swt
OBJS = swt.o Line.o LineSegment.o Rotation.o
CC = g++
CFLAGS = -Wall -g
LIBRARY = -lopencv_core -lopencv_imgproc -lopencv_highgui

swt: $(OBJS)
	$(CC) -o $(PROGRAM) $^ $(LIBRARY)

test: test.o Line.o LineSegment.o Rotation.o
	$(CC) -o swttest $^ $(LIBRARY)

.cpp.o:
	$(CC) $(CFLAGS) $(LIBRARY) -c $<

clean:
	rm -f $(OBJS) $(PROGRAM)
