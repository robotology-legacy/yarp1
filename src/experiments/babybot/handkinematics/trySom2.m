function [net, T] = trySom(varargin)

n = 2;
path = '.\exp5';
% file{1} = 'ball1.txt';
file{1} = 'bowlFilled.txt';
file{2} = 'bowlEmpty.txt';
%file{2} = 'bottle1.txt';
%file{3} = 'rod1.txt';
%file{4} = 'brick1.txt';
%file{5} = 'bigBall1.txt';
%file{6} = 'null1.txt';
%file{7} = 'hard.txt';

color{1} = 'kp';
color{2} = 'ko';
%color{3} = 'k^';
%color{4} = 'kv';
%color{5} = 'ks';
%color{6} = 'k*';
%color{7} = 'ko';

% file1 = 'class1.txt';
% file2 = 'class2.txt';
% file3 = 'class3.txt';

trainPatterns = [];
for i = 1:n
    trainA = load(file{i});
    n1(i) = size(trainA,1);
    ni = 1;
    nf = 32;
    trainA(:,1:16) = trainA(:,1:16)./3.14;
    trainA(:,17:32) = trainA(:,17:32)./255;
    trainPatterns = [trainPatterns; trainA(:,ni:nf)];
end

T = trainPatterns';

if (size(varargin,2) == 2)
    net = varargin{1};
    neurons = varargin{2};
    neurons(1) = neurons;
    neurons(2) = neurons;
else
    neurons(1:2) = 15;

    net = newsom(minmax(T), neurons, 'hextop');
    net.trainParam.epochs = 500;

    net = train(net, T);
end

%% plot net

figure(1), clf, hold on;

pi = 0;
pf = 0;  
    
for i = 1:n
   pi = pf + 1;
   pf = pi + n1(i) - 1;
   P = sim(net, T(:,pi:pf));
   plotData(P, neurons, color{i});
end

figure(1), hold off
figure(1), drawnow;
