function saveTouchForce(filename, force, touch)

fid = fopen(filename, 'r');
if (fid == -1)
    n = 0;
else
    fclose(fid);
    A = load(filename);
    n = size(A,1);
end

fid = fopen (filename, 'a');
if (fid == -1)
    disp('Error opening file!');
    return;
end

tmp = strcat ('Save current touch-force to:', filename);
tmp2 = sprintf('%s (%d)', tmp, n+1);
disp(tmp2);
fprintf(fid, '%f\t', force);
fprintf(fid, '%f\t', touch);
fprintf(fid, '\n');
fclose(fid);