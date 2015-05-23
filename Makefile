all: PI433main

PI433main: PI433.o PI433main.o Device.o MessageQueue.o BluelineDevice.o NexxTechDevice.o LacrossTX141.o Device2262.o
	$(CXX) $(CXXFLAGS) $(LDFLAGS) $+ -o $@ -lwiringPi -lpaho-mqtt3cs
	
clean:
	$(RM) *.o PI433main
