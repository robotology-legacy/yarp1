
default:
	ant dist

clean:
	ant clean

doc:
	gjdoc -s src/ -all -private -d api-docs/
#	gjdoc -s src/ -all -d api-docs/

run:
	java -jar dist/lib/yarp.jar

read:
	java -jar dist/lib/yarp.jar read /read

write:
	java -jar dist/lib/yarp.jar write /write

autowrite:
	ls -R | java -jar dist/lib/yarp.jar write /write /read

connect:
	java -jar dist/lib/yarp.jar connect /write /read

disconnect:
	java -jar dist/lib/yarp.jar disconnect /write /read

server:
	java -jar dist/lib/yarp.jar server



