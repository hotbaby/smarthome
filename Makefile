CC = gcc
CPP = g++
CPPFLAGS = -m32 \
		-Werror \
		-std=c++11 \
		-I . \
		-I ./include/

LDFLAGS = -L ./libs/ -lugwsdk \
		-L ./libs/ -lgearman \
		-L ./libs/ -ljsoncpp \
		-luuid

TARGET = smarthome
OBJS += main.o
OBJS += router.o
OBJS += gearmanWorker.o
OBJS += jsonrpcHandler.o
OBJS += airbox.o
OBJS += common.o
OBJS += airPurifier.o

all:$(TARGET)

smarthome:$(OBJS)
	$(CPP) $(OBJS) $(CPPFLAGS) $(LDFLAGS) -o $@

main.o:main.cpp
	$(CPP) -c main.cpp $(CPPFLAGS) $(LDFLAGS) -o $@

router.o:router.cpp
	$(CPP) -c router.cpp $(CPPFLAGS) $(LDFLAGS) -o $@

gearmanWorker.o:gearmanWorker.cpp
	$(CPP) -c gearmanWorker.cpp $(CPPFLAGS) $(LDFLAGS) -o $@

jsonrpcHandler.o:jsonrpcHandler.cpp
	$(CPP) -c jsonrpcHandler.cpp $(CPPFLAGS) $(LDFLAGS) -o $@

airbox.o:airbox.cpp
	$(CPP) -c airbox.cpp $(CPPFLAGS) $(LDFLAGS) -o $@

common.o:common.cpp
	$(CPP) -c common.cpp $(CPPFLAGS) $(LDFLAGS) -o $@

airPurifier.o:airPurifier.cpp
	$(CPP) -c airPurifier.cpp $(CPPFLAGS) $(LDFLAGS) -o $@

install:
	sudo mkdir -p /etc/smarthome/
	sudo cp ./devicelist.json /etc/smarthome/
	sudo cp smarthome /usr/local/bin/

clean:
	rm $(OBJS)
	rm $(TARGET)


