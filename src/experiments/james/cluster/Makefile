
default:
	echo Hello and welcome to simple-minded cluster management

# make sure debian sources are ok
sources:
	./on_all $(PWD)/update-sources.sh

# make sure necessary debian packages are loaded
install:
	./on_all $(PWD)/update-packages.sh

# remove yarp debian packages
remove:
	./on_all $(PWD)/remove-packages.sh
