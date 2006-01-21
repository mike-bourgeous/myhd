all:
	make -C driver/
	make -C tools/
	make -C tools/dpc/
	make -C tools/firmware/

clean:
	make -C driver/ clean
	make -C tools/ clean
	make -C tools/dpc/ clean
	make -C tools/firmware/ clean

