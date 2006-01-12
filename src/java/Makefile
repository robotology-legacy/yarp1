
EXEC = java -enableassertions -jar dist/lib/yarp.jar

default:
	ant dist

clean:
	ant clean

doc:
	gjdoc -s src/ -all -private -d api-docs/
#	gjdoc -s src/ -all -d api-docs/

run:
	$(EXEC)

read:
	$(EXEC) read /read

write:
	$(EXEC) write /write

autowrite:
	ls -R | $(EXEC) write /write /read

mconnect:
	$(EXEC) connect /mcast://write /read

connect:
	$(EXEC) connect /write /read

udp:
	$(EXEC) connect /write /udp://read

tcp:
	$(EXEC) connect /write /tcp://read

mcast:
	$(EXEC) connect /write /mcast://read

disconnect:
	$(EXEC) disconnect /write /read

server:
	$(EXEC) server

gcj:
	cd src && gcj --main=yarp.os.YarpClient yarp/os/*.java -o ../jyarp

