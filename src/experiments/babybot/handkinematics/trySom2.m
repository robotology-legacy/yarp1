function [net, P, error] = trySom2(varargin)

color = {'ko', 'r*', 'bs', 'g^', 'c^', 'y+'};

n = 2;
path = './exp1/';
file = 'grasping.txt';
filename = sprintf('%s%s', path, file);

data = load(filename);

nj = 16;
ntouch = 17;
nf = 6;

labels = data(:,1);
hand = data(:,2:2+nj-1);
tmpI = 2+nj;
touch = data(:,tmpI:tmpI+ntouch-1);
tmpI = tmpI+ntouch;
force = data(:,tmpI:tmpI+nf-1);

trainPatterns = [hand(:,2:nj) force];

%% divide clusters (for testing)
nClasses = 6;
for i = 1:nClasses
    classes{i} = find(data(:,1) == (i-1));
    for j = 1:size(classes{i},1);
        index = classes{i}(j);
        testSet{i}(j,:) = trainPatterns(index,:);
    end
end

%%%%%%%%%%%%
P = trainPatterns';

neurons(1) = 10;
neurons(2) = 10;

%%%%%%% TRAIN SOM
% epochs = 2000;
% som1 = trainSom(P, neurons, epochs);
% plotNet(net, classes, color, testSet, neurons);
% 
% save 'som1' som1
% 

%%%%%%% TRAIN FF NETWORK
% load 'som1';
% Tc = labels';
% S = sim(som1, P);
% [X, Y, Z, color] = separa(S, neurons, Tc);
% % 
% Z = 2*Z - 1;
% % 
% for i = 1:size(X,2)
%      figure(1), scatter3(X(i),Y(i),Z(i), color{i}), hold on;
% end
% figure(1), hold off;
% 
% P2 = [X;Y];
% T2 = Z;
% 
% netFF = newff(minmax(P2), [4, 1], {'tansig' 'purelin'});
% netFF.trainParam.epochs = 3000;
% netFF = train(netFF, P2, T2);
% Out = sim(netFF, P2);
% 
% % 
% for i = 1:size(X,2)
%      figure(2), scatter3(X(i),Y(i),Out(i), color{i}), hold on;
%      if (Out(i)>=0)
%          Out(i) = 1;
%      end
%      if (Out(i) < 0)
%          Out(i) = -1;
%      end
%      figure(3), scatter3(X(i),Y(i),Out(i), color{i}), hold on;
%  end
%  
% figure(2), hold off;
% figure(3), hold off;
% save 'netFF' netFF
%%%%%%%
net = 0;

% %% TRAIN VQ
% nA = 0;
% nB = 0;
% for i = 1:size(labels,1)
%     if (labels(i) ~= 0)
%         labels(i) = 2;
%         nB=nB+1;
%     else
%         labels(i) = 1;
%         nA = nA+1;
%     end
% end
% 
% Tc = labels';
% T = ind2vec(Tc);
% target = full(T);
% pA = nA/size(labels,1);
% pB = nB/size(labels,1);
% net = newlvq(minmax(P), 25, [pA, pB]);
% net.trainParam.epochs = 300;
% net = train(net, P, T);
% Y = sim(net, P);
% Yc = vec2ind(Y);
% 
% error = Yc-Tc;
error = 0;
%%% train som
function net = trainSom(P, neurons, epochs)
net = newsom(minmax(P), neurons, 'hextop');
net.trainParam.epochs = epochs;
net = train(net, P);

function plotNet(net, classes, color, testSet, neurons)
% % %% plot net
figure(1), clf, hold on;
% % 
pi = 0;
pf = 0;  
% %     
for i = 1:size(classes,2)
%     pi = pf + 1;
%     pf = pi + n1(i) - 1;
      if(size(testSet{i},1)==0)
          continue;
      end
      T = testSet{i}';
      P = sim(net, T);
      plotData(P, neurons, color{i});
      
      disp('class1');
end
%  
figure(1), hold off
figure(1), drawnow;

function [X, Y, Z, c] = separa(P, neurons, Tc)

xx = 1;
yy = 1;
nL = neurons;
for p = 1:size(P,2)
    for n = 1:size(P,1)
        if (P(n,p) == 1)
            X(p) = xx;
            Y(p) = yy;
            if (Tc(p) == 0)
                Z(p) = 0;
                c{p} = 'r*';
            else
                Z(p) = 1;
                c{p} = 'bo';
            end
        end
        xx = xx + 1;
        if (xx > nL(1))
            xx = 1;
            yy = yy + 1;
            if (yy > nL(2))
                yy = 1;
            end
        end
    end
end