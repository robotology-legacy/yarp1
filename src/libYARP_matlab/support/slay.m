function y = slay(machine, command)
%
% slay <machine> <command>
%	
% used in conjunction with YARP ports and windows scripting.
% it slays process with image <command> on machine <machine> where 
% <machine> can be possibly local. It relies on the rkill service
% rkill must be installed configured and tested.
%
%
%	Examples:
%
%	slay ('eolo', 'cmd.exe');
%	slay ('hermes', 'camview.exe');
%
%

if nargin < 2
	disp ('on requires two params: try "help slay"');
	return;
end

s1 = size(machine);
s2 = size(command);

if ~strcmp(class(machine), 'char') || ~strcmp(class(command), 'char') || s1(1) ~= 1 || s2(1) ~= 1
	disp ('params class or size is/are wrong');
	return;
end

% slay doesn't return anything sensible.
system(['slay -n ' machine ' ' command]);

y = 0;
