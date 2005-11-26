function y = on(machine, command)
%
% on <machine> <command>
%	
% used in conjunction with YARP ports and windows scripting.
% it executes on <machine> locally or remotely the given <command> by
% invoking the WSHRemote DCOM component. WSHRemote must be installed,
% activated and enabled.
%
%	Examples:
%
%	on ('eolo', 'cmd');
%	on ('hermes', 'camview -name view/o:img -l -net Net1');
%
%

if nargin < 2
	disp ('on requires two params: try "help on"');
	return;
end

s1 = size(machine);
s2 = size(command);

if ~strcmp(class(machine), 'char') || ~strcmp(class(command), 'char') || s1(1) ~= 1 || s2(1) ~= 1
	disp ('params class or size is/are wrong');
	return;
end

% on doesn't return anything sensible.
system(['on -d -n ' machine ' ' command]);

y = 0;
