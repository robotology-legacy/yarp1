function T = T(x,y,z)

%
% Tranlational matrix
%
% march 2003 -- by nat

T = eye(4,4);

T(1,4) = x;
T(2,4) = y;
T(3,4) = z;
T(4,4) = 1;