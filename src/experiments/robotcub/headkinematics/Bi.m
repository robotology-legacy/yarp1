function B = Bi(q,a,alfa,d)

%
% coordinate transformation matrices
%
% Bi(q,i) 
% q = i-th joint (rad)
% i = coordinate system
%
% alfa, d, a = parameters according to the Denavit Hartenberg 
% 
% 
% September 2002 -- by nat

B = eye(4,4);

B(1,1) = cos(q);
B(1,2) = -sin(q);
B(1,3) = 0;
B(1,4) = a;

B(2,1) = cos(alfa)*sin(q);
B(2,2) = cos(alfa)*cos(q);
B(2,3) = -sin(alfa);
B(2,4) = -sin(alfa)*d;

B(3,1) = sin(alfa)*sin(q);
B(3,2) = sin(alfa)*cos(q);
B(3,3) = cos(alfa);
B(3,4) = cos(alfa)*d;

% B(4,1) = 0;
% B(4,2) = 0;
% B(4,3) = 0;
% B(4,4) = 1;