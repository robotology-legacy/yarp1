function plot_res_force(forces, torques, zoom)

% plot_vector(vector1,vector2,zoom)
% 
% plot 2 3d vectors (vector1 and vector2), where
% vector1(1) = x;
% vector(2) = y;
% vector3(3) = z;
%
% zoom is a scale factor
% Vectors and torques (originally in the jr3 ref. frame) are rotated. Torque are converted
% to the corresponding forces (given the shape of the end effector).

R = zeros(3,3);

res_forces = zeros(1,3);

angle = -pi/4;  % rotation between jr3 ref. frame and our ref. frame

R(2,3) = 1;
R(3,2) = -1;

forces_eq = (RM*torques')';

max_force = zeros(1,3);

max_force(1) = 1;
max_force(2) = 2;
max_force(3) = 2;

res_forces(1) = zoom(1)* forces(1);
res_forces(2) = zoom(2)*forces(2) + zoom(5)*forces_eq(2);
res_forces(3) = zoom(3)*forces(3) + zoom(6)*forces_eq(3);

figure(1), quiver3(0,0,0,res_forces(1), res_forces(2), res_forces(3));
figure(1), hold on;
figure(1), grid on;
figure(1), axis([-max_force(1),max_force(1),-max_force(2),max_force(2),-max_force(3),max_force(3)]);
figure(1), quiver3(0,0,0,res_forces(1), 0,0,'r');   %fx
figure(1), quiver3(0,0,0,0,res_forces(2),0,'r');   %fy
figure(1), quiver3(0,0,0,0,0,res_forces(3),'r');   %fz
figure(1), view(120,45);
figure(1), hold off;

figure(2), quiver(0,0,res_forces(1), res_forces(2));
figure(2), hold on;
figure(2), grid on;
figure(2), axis([-max_force(1),max_force(1),-max_force(2),max_force(2)]);
figure(2), quiver(0,0,res_forces(1), 0,0,'r');   %fx
figure(2), quiver(0,0,0,res_forces(2),0,'r');   %fy
figure(2), view(90,90);
figure(2), hold off;

drawnow;