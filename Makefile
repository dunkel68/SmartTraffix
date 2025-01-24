CC = gcc
CFLAGS = -I/usr/include
LDFLAGS = -lcsfml-graphics -lcsfml-window -lcsfml-system -lm

SRC = main.c lightVehicle.c heavyVehicle.c emergencyVehicle.c trafficLights.c car.c positions.c vehicleQueue.c toeTruck.c bankerAlgo.c
OBJ = $(SRC:.c=.o)
EXE = out

all: $(EXE)

$(EXE): $(OBJ)
	$(CC) $(OBJ) -o $(EXE) $(LDFLAGS)

.c.o:
	$(CC) $(CFLAGS) -c $<

clean:
	rm -f $(OBJ) $(EXE)
