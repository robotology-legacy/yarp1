function computeHandKin (qh,el,az)
%
% q is motor joints
%
% March 2003 by nat
%
%

global X;
global Y;
global Z;

n_fingers = 5;
n_phalanxes = 4;
n_joints = n_fingers*n_phalanxes + 1;

q = zeros(n_joints);
q(1:4) = qh(1:4);
% q(5) is a virtual joint
q(6:8) = -qh(5:7);
% q(9) is a virtual joint
q(10:12) = -qh(8:10);
% q(13) is a virtual joint
q(14:16) = -qh(11:13);
% q(17) is a virtual joint
q(18:20) = -qh(14:16);
% q(21) is a virtual joint

% prepare a sphere
[X, Y, Z] = sphere(20);
X = X*0.03;
Y = Y*0.03;
Z = Z*0.03;

figure(1), CLF;
figure(1), hold on;
figure(1), axis([-20, 20, -5, 25, -5, 10]);
xlabel('x');
ylabel('y');
zlabel('z');
figure(1), view(el,az);
figure(1), grid on;
figure(1), colormap cool;

% finger parameters
 F = [
      10*pi/180, 4, 0, 0;
      pi/2-0*pi/180, 9, 0, -2.5;
      pi/2-0*pi/180, 10, 0, 0;
      pi/2+0*pi/180, 10, 0, 2.5;
      pi/2+0*pi/180, 9, 0, 6];

% DH parameters [a, alfa, d]
DH = [ 
   0, 0, 0;
   0, 0, 0;     % 1
   -4.5, 0, 0;  % 2
   -3.5, 0, 0;
   -3, 0, 0;
   0, 0, 0;
   4.5, 0, 0; %5
   3, 0, 0;
   2.5, 0, 0;
   0, 0, 0;
   6, 0, 0;
   3.5, 0, 0;
   2.5, 0, 0;
   0, 0, 0;
   5, 0, 0;
   3.5, 0, 0;
   2.5, 0, 0;
   0, 0, 0;
   3.5, 0, 0;
   2.5, 0, 0;
   2, 0, 0];    %15

% baseline
R0 = zeros(4,4);
R0(1,1)  = -1;
R0(2,3) = 1;
R0(3,2) = 1;
R0(4,4) = 1;

% kinematics.
P = zeros(4,4,n_joints);
for j = 1:n_joints;
    P(:,:,j) = Bi(q(j), DH(j,1), DH(j,2), DH(j,3));
end

% small phalanxes, virtual joints
for j = 1:n_fingers
    SP(:,:,j) = Bi(0, DH(j,1), DH(j,2), DH(j,3));
end

F0 = zeros(4,4,n_fingers);
for j = 1:n_fingers
    Rt = R(F(j,1));
    Tt = T(F(j,2), F(j,3), F(j,4));
    F0(:,:,j) = Rt*Tt;
end

F1 = zeros(4,4);
F1 = R(3/2*pi)*T(-2.5, 3.5, 0);

PF0 = zeros(4,4,5,n_fingers);
% thumb is different
PF0(:,:,1,1) = R0*F0(:,:,1)*P(:,:,1);    % first phalanx
PF0(:,:,2,1) = PF0(:,:,1,1)*F1*P(:,:,2); % second phalanx
PF0(:,:,3,1) = PF0(:,:,2,1) * P(:,:,3);  % third phalanx
PF0(:,:,4,1) = PF0(:,:,3,1) * P(:,:,4);  % 
PF0(:,:,5,1) = PF0(:,:,4,1) * P(:,:,5);  % last phalanx (virtual)

j = 6;
for f = 2:n_fingers
    PF0(:,:,1,f) = R0*F0(:,:,f)*P(:,:,j);   % first phalanx
    j = j+1;
    PF0(:,:,2,f) = PF0(:,:,1,f) * P(:,:,j); % second phalanx
    j = j+1;
    PF0(:,:,3,f) = PF0(:,:,2,f) * P(:,:,j); % third phalanx
    j = j + 1;
    PF0(:,:,4,f) = PF0(:,:,3,f) * P(:,:,j); % fourth phalanx (virtual)
    j = j + 1;
end

viaPoint = zeros(3, n_fingers);
viaPoint(:,1) = [-F(1,4) 0 0]';
viaPoint(:,2) = [F(2,4) 0 0]';
viaPoint(:,3) = [F(3,4) 0 0]';
viaPoint(:,4) = [F(4,4) 0 0]';
viaPoint(:,5) = [F(5,4) 0 0]';

% build and plot hand wire frame
%%% thumb is different
LXH = [0];
LYH = [0];
LZH = [0];
   
LXH = [LXH viaPoint(1,1)];
LYH = [LYH viaPoint(2,1)];
LZH = [LZH viaPoint(3,1)];

for i=1:5
   LXH = [LXH PF0(1,4,i,1)];
   LYH = [LYH PF0(2,4,i,1)];
   LZH = [LZH PF0(3,4,i,1)];
end

width = 3;

% hand sketch
H = line(LXH, LYH, LZH, 'color', [0, 0, 0]);
set(H,'LineWidth',width);
%%%%%%%%%%%%%%%% end thumb

for f=2:n_fingers
   LXH = [0];
   LYH = [0];
   LZH = [0];
   
   LXH = [LXH viaPoint(1,f)];
   LYH = [LYH viaPoint(2,f)];
   LZH = [LZH viaPoint(3,f)];

   for i=1:n_phalanxes
        LXH = [LXH PF0(1,4,i,f)];
        LYH = [LYH PF0(2,4,i,f)];
        LZH = [LZH PF0(3,4,i,f)];
   end

   width = 3;

   % hand sketch
   H = line(LXH, LYH, LZH, 'color', [0, 0, 0]);
   set(H,'LineWidth',width);
end

% plot <0> ref frame
plot_ref_frame(R0, 0.5, [0,0,1]);
% plot hand ref frames
for f = 1:n_fingers
    for ph = 1:n_phalanxes
        plot_ref_frame(PF0(:,:,ph, f), 0.5, [1, 0, 0]);
    end
end

% head sketch
% H = line(LXL, LYL, LZL);
% set(H,'LineWidth',width);
% H = line(LXR, LYR, LZR);
% set(H,'LineWidth',width);

% origin
% plot_ref_frame (eye(4,4), 30, [1, 0, 0]);

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

% pz = T*[0 0 size 1]';
% px = T*[size 0 0 1]';
% scatter3(px(1), px(2), px(3),'xr');
% scatter3(pz(1), pz(2), pz(3),'xk');