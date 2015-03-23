
CPP = g++
CPPFLAGS = -m32 \
			-Werror \
			-I ../include/ \
			-I ../ \
			-std=c++11

LDFLAGS = -L ../libs/ -ljsoncpp \
			-luuid \
			-lsqlite3

TARGET += jsonrpcTest
TARGET += jsonTest
TARGET += uuid
TARGET += sqliteTest

all:$(TARGET)

jsonTest:jsonTest.cpp
	$(CPP) jsonTest.cpp $(CPPFLAGS) $(LDFLAGS) -o $@

uuid:uuid.cpp
	$(CPP) uuid.cpp $(CPPFLAGS) $(LDFLAGS) -o $@

jsonrpcTest:jsonrpcTest.cpp
	$(CPP) jsonrpcTest.cpp ../jsonrpcHandler.o $(CPPFLAGS) $(LDFLAGS) -o $@

sqliteTest:sqliteTest.cpp
	$(CPP) sqliteTest.cpp $(CPPFLAGS) $(LDFLAGS) -o $@

clean:
	rm $(TARGET)
