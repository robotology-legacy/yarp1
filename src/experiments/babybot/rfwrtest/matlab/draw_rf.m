function hnd = draw_rf(M,C,w,kernel)

[V,E] = eig(M);
d1 = E(1,1);
d2 = E(2,2);
d3 = E(3,3);

fi = linspace(0,2*pi,30)';

teta = linspace(-pi,pi,30);

switch kernel
case 'Gaussian'
	a = sqrt(-2*log(w)/d1);
    b = sqrt(-2*log(w)/d2);
    c = sqrt(-2*log(w)/d3);
case 'BiSquare'
	a = sqrt(2*(1-sqrt(w))/d1);
    b = sqrt(2*(1-sqrt(w))/d2);
    c = sqrt(2*(1-sqrt(w))/d3);
end





Xx_v =  a*cos(fi)*cos(teta);
Xy_v =  b*cos(fi)*sin(teta);
Xz_v =  c*sin(fi)*ones(1,30);

Xx = C(1) + V(1,1)*Xx_v + V(1,2)*Xy_v + V(1,3)*Xz_v;
Xy = C(2) + V(2,1)*Xx_v + V(2,2)*Xy_v + V(2,3)*Xz_v;
Xz = C(3) + V(3,1)*Xx_v + V(3,2)*Xy_v + V(3,3)*Xz_v;
%X=X*V;

hnd = surf(Xx,Xy,Xz);

