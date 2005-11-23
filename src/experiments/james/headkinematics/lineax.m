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
