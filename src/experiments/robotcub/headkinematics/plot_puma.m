function [K] = plot_puma (q, az, el)
%
% Robot kinematics. qh and qa are respectivley head and arm joints (radiants).
% T is the T matrix between end effector and origin
% 
% September 2002 by nat and pasa
%
%

global X;
global Y;
global Z;

degToRad = pi/180;
radToDeg = 180/pi;

arm_offset = [20 33 25 90 30 0];

qpuma = zeros(1,6);

qpuma(1) = -q(1) + arm_offset(1)*degToRad;
qpuma(2) = -q(2) + arm_offset(2)*degToRad;
qpuma(3) = -q(3)+ pi + arm_offset(3)*degToRad;
qpuma(4) = -q(4) + arm_offset(4)*degToRad;
qpuma(5) = q(5) + arm_offset(5)*degToRad;
qpuma(6) = -q(6) + arm_offset(6)*degToRad;


%% DH parameters [a, alfa, d]
DH_puma = [
   0,       0,      250; %13*25.4 --> the waist is a bit shortened here
   0,       pi/2,   (5.9-1)*25.4;
   7.8*25.4,  0,      0;
   0,       -pi/2,  8.75*25.4;
   0,       pi/2,   0;
   0,       -pi/2,  2.2*25.4+60];

% prepare a sphere
[X, Y, Z] = sphere(10); hold on;
X = X * 10;
Y = Y * 10;
Z = Z * 10;

% Puma kinematics.
P1 = Bi(qpuma(1), DH_puma(1,1), DH_puma(1,2), DH_puma(1,3));
P2 = Bi(qpuma(2), DH_puma(2,1), DH_puma(2,2), DH_puma(2,3));
P3 = Bi(qpuma(3), DH_puma(3,1), DH_puma(3,2), DH_puma(3,3));
P4 = Bi(qpuma(4), DH_puma(4,1), DH_puma(4,2), DH_puma(4,3));
P5 = Bi(qpuma(5), DH_puma(5,1), DH_puma(5,2), DH_puma(5,3));
P6 = Bi(qpuma(6), DH_puma(6,1), DH_puma(6,2), DH_puma(6,3));

% puma baseline
PB0 = [
   -1, 0, 0, 0;
   0 ,0 ,-1, 0;
   0, -1, 0, -(3.5)*25.4-10;
   0, 0, 0, 1];

surf(X+PB0(1,4), Y+PB0(2,4), Z+PB0(3,4), Z+PB0(3,4));

P10 = PB0*P1;
P20 = P10*P2;
P30 = P20*P3;
P40 = P30*P4;
P50 = P40*P5;
P60 = P50*P6;

% build puma wire frame
LXP = [0, PB0(1,4), P10(1,4), P20(1,4), P30(1,4), P40(1,4), P50(1,4), P60(1,4)];
LYP = [0, PB0(2,4), P10(2,4), P20(2,4), P30(2,4), P40(2,4), P50(2,4), P60(2,4)];
LZP = [0, PB0(3,4), P10(3,4), P20(3,4), P30(3,4), P40(3,4), P50(3,4), P60(3,4)];

% plot puma ref frames
plot_ref_frame(P10, 30, [1, 0, 0]);
plot_ref_frame(P20, 30, [1, 0, 0]);
plot_ref_frame(P30, 30, [1, 0, 0]);
plot_ref_frame(P40, 30, [1, 0, 0]);
plot_ref_frame(P50, 30, [1, 0, 0]);
plot_ref_frame(P60, 30, [1, 0, 0]);

width = 4;

% puma sketch
H = line(LXP, LYP, LZP, 'color', [1, 0, 1]);
set(H,'LineWidth',width);

% origin
plot_ref_frame (eye(4,4), 30, [1, 0, 0]);

K = P60(1:3,4)';

figure(1), axis([-550, 200, -600, 400, -400, 350]);
figure(1), view(az,el);
figure(1), grid on;
figure(1), colormap cool;
figure(1), hold off;
drawnow;

function plot_ref_frame(T, size, color)
% surface to use as a sphere
global X;
global Y;
global Z;

[tmpX, tmpY, tmpZ] = lineax(T, size);
line(tmpX, tmpY, tmpZ, 'Color', color);
surf(X+T(1,4), Y+T(2,4), Z+T(3,4), Z+T(3,4));