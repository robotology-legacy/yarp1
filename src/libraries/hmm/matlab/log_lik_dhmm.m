function loglik = log_lik_dhmm(data, prior, transmat, obsmat)
% LOG_LIK_DHMM Compute the log-likelihood of a dataset using a discrete HMM
% loglik = log_lik_dhmm(data, prior, transmat, obsmat)

[ncases T] = size(data);
loglik = 0;
for m=1:ncases
  obslik = mk_dhmm_obs_lik(data, obsmat);
  [alpha, xi, LL] = forwards(prior, transmat, obslik);
  loglik = loglik + LL;
end
