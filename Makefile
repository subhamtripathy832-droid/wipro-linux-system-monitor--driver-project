obj-m += src/system_monitor.o

KDIR ?= /lib/modules/$(shell uname -r)/build
PWD  := $(shell pwd)
CXX  ?= g++

APP := system_monitor
APP_SRC := src/system_monitor_app.cpp
CXXFLAGS := -std=c++17 -O2 -Wall -Wextra -pedantic

.PHONY: all driver app clean

all: driver app

driver:
	$(MAKE) -C $(KDIR) M=$(PWD) modules

app:
	$(CXX) $(CXXFLAGS) $(APP_SRC) -o $(APP)

clean:
	$(MAKE) -C $(KDIR) M=$(PWD) clean
	rm -f $(APP)
