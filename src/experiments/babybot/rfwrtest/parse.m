function parse(input, trSet, testSet);

A = load(input);

fidTr = fopen(trSet, 'w');
fidTest = fopen(testSet, 'w');

n = size(A,1);

randomindex=randperm(n);

for i = 1:n-200
    fprintf(fidTr, '%f %f %f %f %f %f\n', A(randomindex(i),1:6));
end

for i = n-199:n
    fprintf(fidTest, '%f %f %f %f %f %f\n', A(randomindex(i),1:6));
end

fclose(fidTr);
fclose(fidTest);