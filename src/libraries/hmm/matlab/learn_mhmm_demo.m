if 1
  O = 2;
  T = 40;
  nex = 100;
  M = 1;
  Q = 5;
else
  O = 8;          %Number of coefficients in a vector 
  T = 420;         %Number of vectors in a sequence 
  nex = 1;        %Number of sequences 
  M = 1;          %Number of mixtures 
  Q = 6;          %Number of states 
end

transmatx = [0.1 0.9 0 0 0; 0 0.1 0.9 0 0; 0 0 0.1 0.9 0; 0 0 0 0.1 0.9; 0.9 0 0 0 0.1];
mux = zeros(O,Q);
for q=1:Q,
   mux(:,q) = [q; q];
end
sigma = zeros (O,O,Q);
for q=1:Q,
   sigma(:,:,q) = [0.01 0; 0 0.01];
end
initial_prob = zeros(Q,1);
initial_prob = 1/Q;
mixmat = ones(Q,M);
mixmat = mk_stochastic (mixmat);

obs = sample_mhmm (T, nex, initial_prob, transmatx, mux, sigma, mixmat);

%fp = fopen ('test_seq.txt', 'r');
%for i=1:nex,
%   for j=1:O,
%      for k=1:T,
%         obs(j,k,i) = fscanf(fp, '%lf ',1);
%      end
%   end
%end

%fclose (fp);

[prior0, transmat0, mixmat0, mu0, Sigma0] =  init_mhmm(obs, Q, M, 'diag', 0);

max_iter = 10;
[LL, prior1, transmat1, mu1, Sigma1, mixmat1] = ...
    learn_mhmm(obs, prior0, transmat0, mu0, Sigma0, mixmat0, max_iter)


%loglik = log_lik_mhmm(obs, prior1, transmat1, mixmat1, mu1, Sigma1);
