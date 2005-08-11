# spec file to create a RPM package (rpm -ba dvgrab.spec)

Name: 		dvgrab
Version: 	1.3
Release: 	1
Packager:	dan@dennedy.org
Copyright: 	2000-2003, Arne Schirmacher, Charles Yates, Dan Dennedy (GPL)
Group: 		Utilities/System
Source0: 	http://kino.schirmacher.de/download/dvgrab-%{version}.tar.gz
URL: 		http://kino.schirmacher.de/
Summary: 	A program to copy Digital Video data from a DV camcorder

Prefix:		/usr
AutoReqProv:	no

%description
dvgrab copies digital video data from a DV camcorder.


%changelog
* Fri May 18 2001 Arne Schirmacher <dvgrab@schirmacher.de>
- minor change for libraw 0.9, bugfix
* Sat Feb 10 2001 Arne Schirmacher <dvgrab@schirmacher.de>
- initial package


%prep


%setup
./configure


%build
make dvgrab


%install
install -s -m 755 -o 0 -g 0 dvgrab /usr/bin/dvgrab
install -s -m 755 -o 0 -g 0 dvgrab.1 /usr/man/man1/dvgrab.1


%clean
rm -rf "${RPM_BUILD_ROOT}"


%post


%postun

%files
/usr/bin/dvgrab
/usr/man/man1/dvgrab.1

%doc README ChangeLog NEWS
