function plot_single_vector(vector1,factor)

% plot_single_vector(vector1)
% 
% plot a 3d vector (vector1), where
% vector1(1) = x;
% vector(2) = y;
% vector3(3) = z;
%

max_force = zeros(1,3);

max_force(1) = 1;
max_force(2) = 1;
max_force(3) = 1;

vector1 = vector1.*factor;

figure(1), quiver3(0,0,0,vector1(1), vector1(2), vector1(3));
figure(1), hold on;
figure(1), grid on;
figure(1), axis([-max_force(1),max_force(1),-max_force(2),max_force(2),-max_force(3),max_force(3)]);
figure(1), quiver3(0,0,0,vector1(1), 0,0,'r');  %fx
figure(1), quiver3(0,0,0,0,vector1(2),0,'r');   %fy
figure(1), quiver3(0,0,0,0,0,vector1(3),'r');   %fz
figure(1), view(120,45);
figure(1), hold off;

drawnow;