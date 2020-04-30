CFLAGS := -g -O2 -Wall -Wextra -Wformat=2
ASFLAGS := -g
LDFLAGS := -no-pie

all: cp57b ls57b

ls57b: ls57b.o lsdir.o getsize.o
cp57b: cp57b.o copyfd.o writeall.o

clean:
	rm -f cp57b ls57b
	rm -f *.o a.out core
