function plot_example(X,Y,Xt,Yt,Yp,s)

% X,Y raw data to train
% Xt,Yt test set
% Yp predicted data.


% plot the raw noisy data
subplot(2,2,1);
plot3(X(:,1),X(:,2),Y,'*');
title('Noisy data samples');

%plot the fitted surface
axis([-1 1 -1 1 -.5 1.5]);
subplot(2,2,2);
[x,y,z]=makesurf([Xt,Yp],sqrt(length(Xt)));
surfl(x,y,z);
axis([-1 1 -1 1 -.5 1.5]);
%title(sprintf('The fitted function: nMSE=%5.3f',nmse));

% plot the true surface
subplot(2,2,3);
[x,y,z]=makesurf([Xt,Yt],sqrt(length(Xt)));
surfl(x,y,z);
axis([-1 1 -1 1 -.5 1.5]);
title('The true function');

%--------------------------------------------------------------------------

function [X,Y,Z]=makesurf(data,nx)
% [X,Y,Z]=makesurf(data,nx) converts the 3D data file data into
% three matices as need by surf(). nx tells how long the row of the
% output matrices are

[m,n]=size(data);

n=0;
for i=1:nx:m,
	n = n+1;
	X(:,n) = data(i:i+nx-1,1);
	Y(:,n) = data(i:i+nx-1,2);
	Z(:,n) = data(i:i+nx-1,3);
end;


