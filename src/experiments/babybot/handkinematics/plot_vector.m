function plot_vector(vector_01, vector_02, zoom)

% plot_vector(vector1,vector2,zoom)
% 
% plot 2 3d vectors (vector1 and vector2), where
% vector1(1) = x;
% vector(2) = y;
% vector3(3) = z;
%
% zoom is a scale factor
%
% Vectors are rotated.

R01 = zeros(3,3);
angle = -pi/4;

RM = zeros(3,3);
RM(2,3) = 1;
RM(3,2) = -1;

vector1 = vector_01;
vector2 = (RM*vector_02')';

max_force = zeros(1,3);
max_torque = zeros(1,3);

max_force(1) = 1;
max_force(2) = 1;
max_force(3) = 1;

max_torque(1) = 1;
max_torque(2) = 1;
max_torque(3) = 1;

figure(3), quiver3(0,0,0,zoom(1)*vector1(1), zoom(2)*vector1(2), zoom(3)*vector1(3));
figure(3), hold on;
figure(3), grid on;
figure(3), axis([-max_force(1),max_force(1),-max_force(2),max_force(2),-max_force(3),max_force(3)]);
figure(3), quiver3(0,0,0,zoom(1)*vector1(1), 0,0,'r');   %fx
figure(3), quiver3(0,0,0,0,zoom(2)*vector1(2),0,'r');   %fy
figure(3), quiver3(0,0,0,0,0,zoom(3)*vector1(3),'r');   %fz
figure(3), view(120,45);
figure(3), xlabel('x');
figure(3), ylabel('y');
figure(3), zlabel('z');
figure(3), hold off;

figure(4), quiver3(0,0,0,zoom(4)*vector2(1), zoom(5)*vector2(2), zoom(6)*vector2(3));
figure(4), hold on;
figure(4), grid on;
figure(4), axis([-max_torque(1),max_torque(1),-max_torque(2),max_torque(2),-max_torque(3),max_torque(3)]);
figure(4), quiver3(0,0,0,zoom(4)*vector2(1), 0,0,'r');   %fx
figure(4), quiver3(0,0,0,0,zoom(5)*vector2(2),0,'r');   %fy
figure(4), quiver3(0,0,0,0,0,zoom(6)*vector2(3),'r');   %fz
figure(4), view(120,45);
figure(4), xlabel('x');
figure(4), ylabel('y');
figure(4), zlabel('z');
figure(4), hold off;
drawnow;