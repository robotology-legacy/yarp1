%function wt = rfsample(X,Y,s) % Simile a rfsample, ma rende i pesi dal training.
%function ypt = rfsample(X,Y,s) % ritorna il risultato della predizione del training
function a = rfsample(X,Y,s) % ritorna la struttura
%function yp = rfsample(X,Y,s) % ritorna il risultato della predizione del simulate

global sdcs;
%format short g;
n=size(X,1);
d=3;

% Inizializzazione rete

%max(abs(X))

  ID = 1;
  if ~exist('s') | isempty(s)
    %rfwr('Init',ID,d,1,0,0,0,1e-4,50,max(abs(X))',max(abs(Y))','test');
    rfwr('Init',ID,d,1,0,0,0,1e-4,50,max(abs(X))', 1 ,'test');
  %  rfwr('Init',ID,d,1,0,0,0,1e-4,50,[0.7,0.7,0.6]', 1 ,'test');
else
    rfwr('Init',ID,s);
  end
  
  
% rfwr('Change',ID,'w_prune',0.2); 
  
  
%   % set some parameters
%   kernel = 'Gaussian';
%   rfwr('Change',ID,'init_D',eye(d)*25); 
%   rfwr('Change',ID,'init_alpha',ones(d)*100);     % this is a safe learning rate
%   rfwr('Change',ID,'w_gen',0.2);                  % more overlap gives smoother surfaces
%   rfwr('Change',ID,'meta',1);                     % meta learning can be faster, but numerical more dangerous
%   rfwr('Change',ID,'meta_rate',100);
%   rfwr('Change',ID,'init_lambda',0.995);
%   rfwr('Change',ID,'final_lambda',0.9999);
%   rfwr('Change',ID,'tau_lambda',0.9999);

% % Permuto gli elementi del vettore, per vedere se training dip da corr.
% 
% vv=randperm(n);
% X = X(vv,:);
% Y = Y(vv);

% Addestramento su ogni campione e testing sul successivo.

for i=1:n-1,
%[ypt,wt(i)] = rfwr('Update',ID,X(i,:)',Y(i,:)'); % la x passata deve risultare colonna!!!
[ypt(i),wt] = rfwr('Update',ID,X(i,:)',Y(i,:)'); % la x passata deve risultare colonna!!!

%[yp,w] = rfwr('Predict',ID,X(i+1,:)',0.001);
[yp(i),w] = rfwr('Predict',ID,X(i+1,:)',0.001);

%Yp(i+1)=yp;
end

a = rfwr('Structure',1);

% for k=1:length(a.rfs),
% %a.rfs(k).D 
% k
% sum(sum(a.rfs(k).D))
% end


% disp(sprintf('RFs: %d',length(sdcs(ID).rfs)));
% 
% 
%  Yp(1)=Y(1);
%  Yp=Yp';
% % 
%  diff= Y-Yp;
% % 
%  e1 = max(abs(diff))
%  e2 = sum(diff.^2)
%  e2bis = sum(diff(floor(n/2):end).^2); % L'e2 calcolato solo sull'ultima metà dei dati
%  e2pp=mean(abs(diff(floor(n/2):end))); % Media err2 sull'ultima metà dei dati. 
% % 
%  disp(sprintf('e1=%5.3f e2=%5.3f e2bis=%5.3f e2pp=%5.3f',e1,e2,e2bis,e2pp)); 
% % 
%  plot(abs(diff));
% ------------------- 

% 
% h=[1:n];
% plot(h,Y,h,Yp,'r');

hold on

for i=1:length(sdcs(ID).rfs),
       
    hnd = draw_rf(sdcs(ID).rfs(i).D,sdcs(ID).rfs(i).c,0.1,sdcs(ID).kernel);
    set(hnd,'edgecolor','none','facealpha',0.1);
  %  set(hnd,'edgecolor',[0.1 0.4 0.7],'facealpha',0.1);
    C=sdcs(ID).rfs(i).c;
    plot3(C(1),C(2),C(3),'ko')
    
end



for i=1:n,
    x = X(i,:);
    plot3(x(1),x(2),x(3),'rx');
    
end

% for i=1:4,
% [V E]= eig(sdcs(ID).rfs(i).D);
% V
% end


disp(sprintf('RFs: %d',length(sdcs(ID).rfs)));

hold off;
    




