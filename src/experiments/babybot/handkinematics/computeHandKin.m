function computeHandKin (qh, qt, wrist, el, az)
%
% q is motor joints
%
% March 2003 by nat
%
% Sept 2004 by nat, included wrist
% Oct 2004 by nat: this code is a mess

global XSF;
global YSF;
global ZSF;

global XCYL;
global YCYL;
global ZCYL;

global XCYLink;
global YCYLink;
global ZCYLink;
global colorCYL;
global colorLink

n_fingers = 5;
n_phalanxes = 4;
n_joints = n_fingers*n_phalanxes + 1;    % thumb has one more;

t1 = 4.5;
t2 = -2;
alfa = atan2(t1,t2);
a = sqrt(t1*t1+t2*t2);

q = zeros(n_joints);
q(1) = qh(1);
q(2:4) = qh(2:4);
% q(5) is a virtual joint
q(6:8) = -qh(5:7);
% q(9) is a virtual joint
q(10:12) = -qh(8:10);
% q(13) is a virtual joint
q(14:16) = -qh(11:13);
% q(17) is a virtual joint
q(18:20) = -qh(14:16);
% q(21) is a virtual joint

% prepare a sphere for tactile sensors
[XSF, YSF, ZSF] = sphere(8);
XSF = XSF*0.4;
YSF = YSF*0.4;
ZSF = ZSF*0.4;
% prepare a cylinedr for joint ref frame
[XCYL, YCYL, ZCYL] = cylinder(0.1, 20);
XCYL = XCYL*2;
YCYL = YCYL*2;
ZCYL = (ZCYL-0.5)*2;
colorCYL = ZCYL;
colorCYL(:) = 1;

[XCYLink, YCYLink, ZCYLink] = cylinder(0.6, 20);
colorLink = ZCYLink;
colorLink(:) = 1;

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
   0, 0, 0;     % thumb 1
   0, 0, 0;     % 
   -4.5, 0, 0;  % 2
   -3.5, 0, 0;  % 3
   -3, 0, 0;    % last thumb phalanx, virtual joint
   0, 0, 0;     % index
   4.5, 0, 0;   %
   3, 0, 0;
   2.5, 0, 0;   % index, virtual
   0, 0, 0;     % medium
   6, 0, 0;
   3.5, 0, 0;
   2.5, 0, 0;
   0, 0, 0;     % ring
   5, 0, 0;
   3.5, 0, 0;
   2.5, 0, 0;
   0, 0, 0;     % small
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
F1 = R(3/2*pi)*T(t2, t1, 0);

PF0 = zeros(4,4,5,n_fingers);
% thumb is different
PF0(:,:,1,1) = R0*F0(:,:,1)*P(:,:,1);       % first phalanx
PF0(:,:,2,1) = PF0(:,:,1,1)*F1*P(:,:,2);
PF0(:,:,3,1) = PF0(:,:,2,1) * P(:,:,3); 
PF0(:,:,4,1) = PF0(:,:,3,1) * P(:,:,4);
PF0(:,:,5,1) = PF0(:,:,4,1) * P(:,:,5); % last phalanx (virtual)

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

% 
% viaPoint = zeros(3, n_fingers);
% viaPoint(:,1) = [-F(1,4) 0 0]';
% viaPoint(:,2) = [F(2,4) 0 0]';
% viaPoint(:,3) = [F(3,4) 0 0]';
% viaPoint(:,4) = [F(4,4) 0 0]';
% viaPoint(:,5) = [F(5,4) 0 0]';

% build and plot hand wire frame
%%% thumb is different
% plot_link(R0, [0,0,0]', viaPoint(:,1));

plot_link(PF0(:,:,1,1)*R(3/2*pi)*Bi(alfa, 0, 0, 0), [0, 0, 0]', [a 0 0]');    %% check this
for i=2:5
   plot_link(PF0(:,:,i,1), [0, 0, 0]', [DH(i+1,1) 0 0]');
end
%%% the other fingers
dh = 6;
for f=2:n_fingers
    % plot_link(R0, [0,0,0]', viaPoint(:,f));
    dh = dh+1;
    plot_link(PF0(:,:,1,f), [0 0 0]', DH(dh,1));
    for i=1:3
       plot_link(PF0(:,:,i,f), [0, 0, 0]', [DH(dh,1) 0 0]');
       dh = dh+1;
    end
end

%%% %%% %%% %%% %%% 
% plot touch sensors
% 1:5 -> palm
plot_sensor(R0, [1.3 0.6 7], qt(5)/255, 1);
plot_sensor(R0, [-2 0.6 7], qt(3)/255, 1);
plot_sensor(R0, [-0.5 0.6 4.5], qt(2)/255, 1);
plot_sensor(R0, [-3.2 0.6 4.5], qt(4)/255, 1);
plot_sensor(R0, [-3 0.6 1.8], qt(1)/255, 1);

%%%%%%%%%%%%%%%%%
%% FINGERS
sizeT = 0.7;
ts = 6;
dh_index = 3;
for ph = 2:n_phalanxes
    v = qt(ts)/255;
    plot_sensor(PF0(:,:,ph,1), [DH(dh_index,1)/2 -0.6 0], v, sizeT);
    ts = ts+1;
    dh_index = dh_index + 1;
end

for f = 2:(n_fingers-1) % small finger has no sensors
    dh_index = dh_index+1;    % skip last phalanx
    for ph = 1:(n_phalanxes-1)
         v = qt(ts)/255;
         plot_sensor(PF0(:,:,ph,f), [DH(dh_index,1)/2 0.6 0], v, sizeT);
         ts = ts+1;
         dh_index = dh_index + 1;
    end
end

% plot <0> ref frame
plot_ref_frame(R0, 0.5, [0,0,1]);
% plot hand ref frames
% thumb is different
for ph = 1:n_phalanxes
    plot_ref_frame(PF0(:,:,ph, 1), 0.5, [1, 0, 0]);
end
for f = 2:n_fingers
    for ph = (1:n_phalanxes-1)
        plot_ref_frame(PF0(:,:,ph, f), 0.5, [1, 0, 0]);
    end
end

zoom(1:6) = 200;
plot_wrist_force(wrist(1:3), wrist(4:6), zoom);

figure(1), hold off;
drawnow;

function plot_link(T, start, fend)
global XCYLink;
global YCYLink;
global ZCYLink;
global colorLink;

tmpXCyl = XCYLink;
tmpYCyl = YCYLink;
tmpZCyl = ZCYLink;

n  = size(XCYLink, 1);
m = size(YCYLink, 2);
tmpP = zeros(4, 1);
tmpT = zeros(4, 1);
for j = 1:m
    tmpP(1) = ZCYLink(1,j)*start(1);
    tmpP(2) = YCYLink(1,j);%*start(2);
    tmpP(3) = XCYLink(1,j);%*start(3);
    tmpP(4) = 1;
    tmpT = T*tmpP;
    tmpXCyl(1,j) = tmpT(1);
    tmpYCyl(1,j) = tmpT(2);
    tmpZCyl(1,j) = tmpT(3);
end
for j = 1:m
    tmpP(1) = ZCYLink(2,j)*fend(1);
    tmpP(2) = YCYLink(2,j);%*fend(2);
    tmpP(3) = XCYLink(2,j);%*fend(3);
    tmpP(4) = 1;
    tmpT = T*tmpP;
    tmpXCyl(2,j) = tmpT(1);
    tmpYCyl(2,j) = tmpT(2);
    tmpZCyl(2,j) = tmpT(3);
end
H = surf(tmpXCyl, tmpYCyl, tmpZCyl, XCYLink);
shading interp; %colorLink
% set(H, 'EdgeColor', [0 0 1]);

function plot_ref_frame(T, width, color)
% surface to use as a sphere
global XCYL;
global YCYL;
global ZCYL;
global colorCYL;

tmpXCyl = XCYL;
tmpYCyl = YCYL;
tmpZCyl = ZCYL;

%% plot reference frame as three lines
% [tmpX, tmpY, tmpZ] = lineax(T, width);
% line(tmpX, tmpY, tmpZ, 'Color', color);

n  = size(XCYL, 1);
m = size(XCYL, 2);
tmpP = zeros(4, 1);
tmpT = zeros(4, 1);
for i = 1:n
    for j = 1:m
        tmpP(1) = XCYL(i,j);
        tmpP(2) = YCYL(i,j);
        tmpP(3) = ZCYL(i,j);
        tmpP(4) = 1;
        tmpT = T*tmpP;
        tmpXCyl(i,j) = tmpT(1);
        tmpYCyl(i,j) = tmpT(2);
        tmpZCyl(i,j) = tmpT(3);
    end
end

H = surf(tmpXCyl, tmpYCyl, tmpZCyl, XCYL); % colorCYL
set(H, 'EdgeColor', [0 0 1]);

function plot_sensor(R, pos, v, r)
if (v>0)
    color = HSV2RGB([0 v 1]);
else
    color = [0 0 0];
end
Np = 20;
t = 0:(2*pi/Np):2*pi;
xx = r*cos(t) + pos(1);
yy = t.*0 + pos(2);
zz = r*sin(t) + pos(3);
pp = ones(1,size(xx,2));
A = R* [xx;yy;zz;pp];
figure(1), fill3(A(1,:), A(2,:), A(3,:), color);

% pz = T*[0 0 width 1]';
% px = T*[width 0 0 1]';
% scatter3(px(1), px(2), px(3),'xr');
% scatter3(pz(1), pz(2), pz(3),'xk');


% % build and plot hand wire frame
% %%% thumb is different
% LXH = [0];
% LYH = [0];
% LZH = [0];
%    
% LXH = [LXH viaPoint(1,1)];
% LYH = [LYH viaPoint(2,1)];
% LZH = [LZH viaPoint(3,1)];
% 
% for i=1:5
%    LXH = [LXH PF0(1,4,i,1)];
%    LYH = [LYH PF0(2,4,i,1)];
%    LZH = [LZH PF0(3,4,i,1)];
% end
% 
% width = 3;
% 
% % hand sketch
% H = line(LXH, LYH, LZH, 'color', [0, 0, 0]);
% set(H,'LineWidth',width);
% %%%%%%%%%%%%%%%% end thumb
% 
% for f=2:n_fingers
%    LXH = [0];
%    LYH = [0];
%    LZH = [0];
%    
%    LXH = [LXH viaPoint(1,f)];
%    LYH = [LYH viaPoint(2,f)];
%    LZH = [LZH viaPoint(3,f)];
% 
%    for i=1:n_phalanxes
%         LXH = [LXH PF0(1,4,i,f)];
%         LYH = [LYH PF0(2,4,i,f)];
%         LZH = [LZH PF0(3,4,i,f)];
%    end
% 
%    width = 3;
% 
%    % hand sketch
%    H = line(LXH, LYH, LZH, 'color', [0, 0, 0]);
%    set(H,'LineWidth',width);
% end

function plot_wrist_force(vector_01, vector_02, zoom)

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

figure(1), quiver3(0,0,0,zoom(1)*vector1(1), zoom(2)*vector1(2), zoom(3)*vector1(3), 'b');
% figure(3), quiver3(0,0,0,zoom(1)*vector1(1), 0,0,'r');   %fx
% figure(3), quiver3(0,0,0,0,zoom(2)*vector1(2),0,'r');   %fy
% figure(3), quiver3(0,0,0,0,0,zoom(3)*vector1(3),'r');   %fz
figure(1), quiver3(0,0,0,zoom(4)*vector2(1), zoom(5)*vector2(2), zoom(6)*vector2(3), 'r');
% figure(4), quiver3(0,0,0,zoom(4)*vector2(1), 0,0,'r');   %fx
%figure(4), quiver3(0,0,0,0,zoom(5)*vector2(2),0,'r');   %fy
%figure(4), quiver3(0,0,0,0,0,zoom(6)*vector2(3),'r');   %fz
%figure(4), view(120,45);
%figure(4), xlabel('x');
%figure(4), ylabel('y');
%figure(4), zlabel('z');
