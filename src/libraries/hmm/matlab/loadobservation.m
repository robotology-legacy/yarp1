fp = fopen ('observation.txt','rt');
   
data = zeros(O,T,nex);
for n=1:nex,
   for t=1:T,
		for o=1:O,
         data(o,t,n) = fscanf(fp, '%f', 1);
      end
   end
end

fclose (fp);
