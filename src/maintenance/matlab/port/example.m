%
% example
%	
%	example of using yarp ports within matlab.
%
%

id = port('create', 'vector', 0, 'mcast');
if id == -1,
	disp('troubles creating port');
	return;
end

e = port('register', id, '/matlab/i', 'default');
if e == -1,
	disp('cannot register port');
	return;
end

%
%
porter ('/handcontrol/o:status', '/matlab/i');

%
% and now prints a few values.

for i=1:10,
	port('read', id)
end

porter ('/handcontrol/o:status', '!/matlab/i');

%
% destroy the whole thing.

e = port('unregister', id);
e = port('destroy', id);

%
% unload the MEX DLL.
clear port;


