
EXEC = java -enableassertions -jar dist/lib/yarp.jar

default:
	ant dist

clean:
	ant clean

pub-docs:
	gjdoc -s src/ -all -d api-docs/

docs:
	gjdoc -s src/ -all -private -d api-docs/

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
	$(EXEC) connect /write udp://read

tcp:
	$(EXEC) connect /write tcp://read

mcast:
	$(EXEC) connect /write mcast://read

disconnect:
	$(EXEC) disconnect /write /read

server:
	$(EXEC) server

gcj:
	cd src && gcj --main=yarp.os.YarpClient yarp/os/*.java -o ../jyarp

gcjh:
	cd src && gcj -C yarp/os/*.java
	cd src && for f in `find . -iname "*.class"`; do \
		name=`echo $$f | sed "s/\.class//" | sed "s/^\.\///"`; \
		echo gcjh $$name; \
		gcjh $$name; \
	done
	cd src && find . -iname "*.class" -exec mv {} /tmp \;
	@echo Hopefully, header files have been generated for the java files

gcc:
	cd src && gcj yarp/os/*.java yarp/os/*.cpp -o ../cyarp -lstdc++

test:
	java -enableassertions -cp dist/lib/yarp.jar:/usr/share/java/junit.jar junit.textui.TestRunner yarp.os.TextCarrierTest

