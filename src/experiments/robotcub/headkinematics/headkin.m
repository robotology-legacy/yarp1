function y = headkin (q)
%
% computes the head forward kinematics fot the robotcub head.
%
%

global X;
global Y;
global Z;

% prepare a sphere for drawing
[X, Y, Z] = sphere(5); hold on;
X = X * 2;
Y = Y * 2;
Z = Z * 2;

degToRad = pi/180;
radToDeg = 180/pi;
length = 84.4;

theta = q(1) * degToRad;
phi = q(2) * degToRad;

neck = eye(4,4);
neck(1,1) = (1-cos(phi)) * sin(theta)^2 + cos(phi);
neck(1,2) = -(1-cos(phi)) * sin(theta) * cos(theta);
neck(1,3) = cos(theta) * sin(phi);
neck(2,1) = neck(1,2);
neck(2,2) = (1-cos(phi)) * cos(theta)^2 + cos(phi);
neck(2,3) = sin(theta) * sin(phi);
neck(3,1) = -cos(theta) * sin(phi);
neck(3,2) = -sin(theta) * sin(phi);
neck(3,3) = cos(phi);

if abs(phi) > 1e-5,
    neck(1,4) = -(length/phi * cos(phi) - length/phi) * cos(theta);
    neck(2,4) = -(length/phi * cos(phi) - length/phi) * sin(theta);
    neck(3,4) = length/phi * sin(phi);
else
    neck(1,4) = 0;
    neck(2,4) = 0;
    neck(3,4) = length;
end

% origin
figure(1), plot_ref_frame (eye(4,4), 30, [1, 0, 0]);
figure(1), plot_ref_frame (neck, 30, [1, 0, 0]);

figure(1), axis([-100, 100, -100, 100, -100, 100]);
figure(1), grid on;
figure(1), colormap cool;
figure(1), xlabel('x'), ylabel('y'), zlabel('z');
figure(1), hold off;
drawnow;

y = neck;


function [LX, LY, LZ] = lineax (T, sz)
%
% Given a hom transform, returns the lines for drawing the frame.
% frame size is given by sz
%
LX = [
   T(1,4), 
   T(1,4) + T(1,1) * sz, 
   T(1,4), 
   T(1,4) + T(1,2) * sz, 
   T(1,4), 
   T(1,4) + T(1,3) * sz]; 
LY = [
   T(2,4), 
   T(2,4) + T(2,1) * sz, 
   T(2,4), 
   T(2,4) + T(2,2) * sz, 
   T(2,4), 
   T(2,4) + T(2,3) * sz]; 
LZ = [
   T(3,4), 
   T(3,4) + T(3,1) * sz, 
   T(3,4), 
   T(3,4) + T(3,2) * sz, 
   T(3,4), 
   T(3,4) + T(3,3) * sz]; 


function plot_ref_frame(T, size, color)
% surface to use as a sphere
global X;
global Y;
global Z;

T(:,1) = 0;
T(:,2) = 0;
[tmpX, tmpY, tmpZ] = lineax(T, size);
line(tmpX, tmpY, tmpZ, 'Color', color);
surf(X+T(1,4), Y+T(2,4), Z+T(3,4), Z+T(3,4));