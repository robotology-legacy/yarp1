function y = porter(name1, name2)
%
% porter <name1> <name2>
%	
% used in conjunction with YARP ports, it calls the output port <name1> to operate
% the input port <name2> according to a command code (see below). 
% <name1> gets called (through a TCP channel) and triggered to contact <name2>. 
% Both arguments are strings.
%
%	Examples:
%		porter /matlab/o:int /tester/i:int		% connect 
%		porter /matlab/o:int !/tester/i:int		% remove connection
%
%

y = port('porter', name1, name2);

