CFLAGS = -Wall -g
LDFLAGS = -lm

all: closest generate_points

closest: closest.o serial_closest.o parallel_closest.o point_utilities.o
	gcc $(CFLAGS) -o $@ $^ $(LDFLAGS)

generate_points: generate_points.o
	gcc $(CFLAGS) -o $@ $^

serial_closest.o: serial_closest.c serial_closest.h point.h point_utilities.h
	gcc $(CFLAGS) -c $<

parallel_closest.o: parallel_closest.c parallel_closest.h point.h serial_closest.h point_utilities.h
	gcc $(CFLAGS) -c $<

point_utilities.o: point_utilities.c point_utilities.h point.h
	gcc $(CFLAGS) -c $<

closest.o: closest.c point.h point_utilities.h serial_closest.h parallel_closest.h
	gcc $(CFLAGS) -c $<

generate_points.o: generate_points.c point.h
	gcc $(CFLAGS) -c $<

clean:
	rm -f *.o closest generate_points