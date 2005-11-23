function [K] = compute_haed (q)
%
% Head kinematics. 
% Result is a row
% 
% September 2002 by nat and pasa
%
%

qhead = zeros(1,6);

qhead(1) = -q(1);
qhead(2) = q(2) - pi/2;
qhead(3) = q(3) + pi/2;
qhead(4) = pi/2;
qhead(5) = -q(4) + pi/2;
qhead(6) = q(5) + pi/2;

%% DH parameters [a, alfa, d]
DH_l = [ 
   0, 0, 0;     % 0
   0, -pi/2, 0;  % 1
   125, 0, 0;   % 2
   0, pi/2, 0; % 3
   -71.5, 0, 0];    % 4

DH_r = [
   0, 0, 0;     % 0
   0, -pi/2, 0;  % 1
   125, 0, 0;   % 2
   0, pi/2, 0; % 3
   71.5, 0, 0];    % 4

% head kinematics
H1 = Bi(qhead(1), DH_l(1,1), DH_l(1,2), DH_l(1,3));
H2 = Bi(qhead(2), DH_l(2,1), DH_l(2,2), DH_l(2,3));
H3 = Bi(qhead(3), DH_l(3,1), DH_l(3,2), DH_l(3,3));
H4 = Bi(qhead(4), DH_l(4,1), DH_l(4,2), DH_l(4,3));
H5 = Bi(qhead(5), DH_r(5,1), DH_r(5,2), DH_r(5,3));

H6 = Bi(qhead(6), DH_l(5,1), DH_l(5,2), DH_l(5,3));

% head baseline
HB = eye(4,4);
HB(3,4) = 133;

H10 = HB*H1;
H20 = H10*H2;
H30 = H20*H3;
H40 = H30*H4;
H50 = H40*H5;
H60 = H40*H6;

K = fixation_point1(H50, H60);

K = K';

function f = fixation_point1(T1, T2)
% compute fixation point by means of the pseudo inverse
A = [ 
   1, 0, 0, -T1(1,1), 0;
   0, 1, 0, -T1(2,1), 0;
   0, 0, 1, -T1(3,1), 0;
   1, 0, 0, 0, -T2(1,1);
   0, 1, 0, 0, -T2(2,1);
   0, 0, 1, 0, -T2(3,1)];

b = [T1(1,4); T1(2,4); T1(3,4); T2(1,4); T2(2,4); T2(3,4)];

res = pinv(A) * b;

if (res(4) < 0) | (res(5) < 0),
   f = [0; 0; 0];
else
   f = res(1:3);
end