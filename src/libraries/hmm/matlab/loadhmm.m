fp = fopen ('test_2.txt', 'rt');

Q = fscanf (fp, '%d', 1);
M = fscanf (fp, '%d', 1);
O = fscanf (fp, '%d', 1);

dummy = fscanf (fp, '%d', 1);		% mixture type	
dummy = fscanf (fp, '%d', 1);		% left-right

prior0 = fscanf (fp, '%f', Q);
transmat0 = fscanf (fp, '%f', [Q, Q]);
mixmat0 = fscanf (fp, '%f', [Q, M]);

for q=1:Q,
   for m=1:M,
      mu0(:,q,m) = fscanf (fp, '%f', O);
      sigma0(:,:,q,m) = fscanf (fp, '%f', [O,O]);
      sigma0(:,:,q,m) = diag(diag(sigma0(:,:,q,m)));
   end
end

fclose(fp);