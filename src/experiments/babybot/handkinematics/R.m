function R = R(theta)

%
% rotation around y 
% march 2003 -- by nat

R = eye(4,4);

R(1,1) = cos(theta);
R(1,2) = 0;
R(1,3) = -sin(theta);
R(1,4) = 0;

R(2,1) = 0;
R(2,2) = 1;
R(2,3) = 0;
R(2,4) = 0;

R(3,1) = sin(theta);
R(3,2) = 0;
R(3,3) = cos(theta);
R(3,4) = 0;

% R(4,1) = 0;
% R(4,2) = 0;
% R(4,3) = 0;
% R(4,4) = 1;