function appendToFile(qh, qtouch, qforce)

% ask file name
filename = 'grasping.txt';
a = 0;
a = input('was grasped (0 = no, anything else = yes) ? ');

%% check file size
fid = fopen(filename, 'r');
if (fid == -1)
    n = 0;
else
    fclose(fid);
    A = load(filename);
    n = size(A,1);
end

%% add
fid = fopen (filename, 'a');
if (fid == -1)
    disp('Error opening file!');
    return;
end

tmp = strcat ('Save current posture to:', filename);
tmp2 = sprintf('%s (%d)', tmp, n+1);
disp(tmp2);
fprintf(fid, '%d\t', a);
fprintf(fid, '%f\t', qh);
fprintf(fid, '%f\t', qtouch);
fprintf(fid, '%f\t', qforce);
fprintf(fid, '\n');
fclose(fid);