function [K] = compute_puma (q)
%
% Compute puma kinematics. 
% result is a row
% 
% September 2002 by nat and pasa
%
%

degToRad = pi/180;
radToDeg = 180/pi;

arm_offset = [20 33 25 90 30 0];

qpuma = zeros(1,6);

qpuma(1) = -q(1) + arm_offset(1)*degToRad;
qpuma(2) = -q(2) + arm_offset(2)*degToRad;
qpuma(3) = -q(3)+pi + arm_offset(3)*degToRad;
qpuma(4) = -q(4) + arm_offset(4)*degToRad;
qpuma(5) = q(5) + arm_offset(5)*degToRad;
qpuma(6) = -q(6) + arm_offset(6)*degToRad;

DH_puma = [
   0,       0,      250; %13*25.4 --> the waist is a bit shortened here
   0,       pi/2,   (5.9-1)*25.4;
   7.8*25.4,  0,      0;
   0,       -pi/2,  8.75*25.4;
   0,       pi/2,   0;
   0,       -pi/2,  2.2*25.4+60];

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

P10 = PB0*P1;
P20 = P10*P2;
P30 = P20*P3;
P40 = P30*P4;
P50 = P40*P5;
P60 = P50*P6;

K = P60(1:3,4)';