function error = rfsample3D(X,Y,s)

global sdcf;

n=length(X);
d=3;

% Inizializzazione rete

%MAX_X = max(abs(X))
%MAX_Y = max(abs(Y))
MAX_X = [0.5, 0.5, 700];
MAX_Y = [0.5, 1, 1];

  ID = 1;
  if ~exist('s') | isempty(s)
    rfwr('Init',ID,d,d,0,1,250,0.0000000001,50,MAX_X',MAX_Y','test');

% Importante: gli X devono essere compresi tra -1 e 1, sennò niente output.
    
else
    rfwr('Init',ID,s);
  end

  % set some parameters
  kernel = 'Gaussian';
%   rfwr('Change',ID,'init_D',eye(d)*25); 
%   rfwr('Change',ID,'init_alpha',ones(d)*100);     % this is a safe learning rate
%   rfwr('Change',ID,'w_gen',0.2);                  % more overlap gives smoother surfaces
%   rfwr('Change',ID,'meta',1);                     % meta learning can be faster, but numerical more dangerous
%   rfwr('Change',ID,'meta_rate',100);
%   rfwr('Change',ID,'init_lambda',0.995);
%   rfwr('Change',ID,'final_lambda',0.9999);
%   rfwr('Change',ID,'tau_lambda',0.9999);


% Addestramento e testing su ogni campione

for i=1:n-1,
[ypt,w] = rfwr('Update',ID,X(i,:)',Y(i,:)'); % la x passata deve risultare colonna!!!
[yp,w] = rfwr('Predict',ID,X(i+1,:)',0.001);
Yp(i+1,:)=yp(:,1)';
end

Yp(1,:) = Y(1,:);

diff = Y - Yp; % 208x3

subplot(3,1,1);
plot(abs(diff(:,1)));
subplot(3,1,2);
plot(abs(diff(:,2)));
subplot(3,1,3);
plot(abs(diff(:,3)));

error = sqrt(sum(diff.^2, 2));

% Calcolo gli errori a partire dalla seconda parte dei dati. 
%diff =diff(51:200,:);

diff = diff(floor(n/2):end,:);
sumquad =diag(diff*diff'); % Ottengo un vettore colonna cost. dalle somme dei quadrati delle singole righe -> moduli singole righe
e1tot=max(sumquad);
e2tot=sum(sumquad);

diffcol1=diff(:,1);
diffcol2=diff(:,2);
diffcol3=diff(:,3);

e1c1=max(abs(diffcol1));
e1c2=max(abs(diffcol2));
e1c3=max(abs(diffcol3));

e2c1=sum(diffcol1.^2);
e2c2=sum(diffcol2.^2);
e2c3=sum(diffcol3.^2);

disp(sprintf('e1tot=%5.3f e2tot=%5.3f \ne1x=%5.3f e2x=%5.3f \ne1y=%5.3f e2y=%5.3f \ne1z=%5.3f e2z=%5.3f ',e1tot,e2tot,e1c1,e2c1,e1c2,e2c2,e1c3,e2c3));

h=[1:n];

figure;
subplot(3,1,1);
plot(h,Yp(:,1),h,Y(:,1),'r--');
subplot(3,1,2);
plot(h,Yp(:,2),h,Y(:,2),'r--');
subplot(3,1,3);
plot(h,Yp(:,3),h,Y(:,3),'r--');

struct = rfwr('Structure', ID);