function lpColored = reconstructColor(lp)
% reconstruct color
% by nat Apr. 2004
% Note: image input is assumed to be a raw logpolar image (uint8) 152x252x1

YARP_ROOT = 'd:/nat/yarp';
fileName = '/conf/WeightsMap02_P8.gio';

% read map from file
[lutRow, lutCol, weights] = readColorLUT(sprintf('%s%s', YARP_ROOT, fileName));

% reconstruct color
lpColored = zeros(size(lp,1), size(lp,2), 3);

N = size(lp,1);
M = size(lp,2);

i = 1;
tmp = 0;
for r = 1:N
    for c = 1:M
        lpColored(r,c,3) = weights(i,1)*(double(lp(lutRow(i,1),lutCol(i,1))));
        lpColored(r,c,2) = weights(i,2)*(double(lp(lutRow(i,2),lutCol(i,2))));
        lpColored(r,c,1) = weights(i,3)*double(lp(lutRow(i,3),lutCol(i,3)));
        i = i+1;
        
        tmp = weights(i,1)*(double(lp(lutRow(i,1),lutCol(i,1))));
        lpColored(r,c,3) = lpColored(r,c,3) +  tmp;
        tmp = weights(i,2)*(double(lp(lutRow(i,2),lutCol(i,2))));
        lpColored(r,c,2) = lpColored(r,c,2) + tmp;
        tmp = weights(i,3)*(double(lp(lutRow(i,3),lutCol(i,3))));
        lpColored(r,c,1) = lpColored(r,c,1) + tmp;
        i = i+1;
    end
end

lpColored = uint8(lpColored);


function [lutRow, lutCol, lutW] = readColorLUT(fileName)

%% read raw map from file
nRho = 152;
nTheta = 252;
pixNumb = 2;

fid = fopen(fileName, 'r', 'l');

lutSize = nRho*nTheta*pixNumb*3;
positions = fread(fid, lutSize, 'ushort',2+4);  % skip struct padding and float

frewind(fid);   % rewind file
dummy = fread(fid, 1, 'ushort',2);            %skip first+struct padding
weigths = fread(fid, lutSize, 'float',2+2);     % skip ushort + struct padding
fclose(fid);

%%%%%%%%%%%%% build LUTs
nElem = 2*nRho*nTheta;
offset1 = nRho*nTheta*2;
offset2 = nRho*nTheta*4;
pad = 4;

lutCol = zeros(nElem,3);
lutRow = zeros(nElem,3);
lutW = zeros(nElem,3);

lutRow(:,1) = floor((positions(1:nElem))/(nTheta+pad))+1;
lutCol(:,1) = positions(1:nElem)-(lutRow(:,1)-1)*(nTheta+pad)+1;

lutRow(:,2) = floor(positions(1+offset1:nElem+offset1)/(nTheta+pad))+1;
lutCol(:,2) = positions(1+offset1:nElem+offset1)-(lutRow(:,2)-1)*(nTheta+pad)+1;

lutRow(:,3) = floor(positions(1+offset2:nElem+offset2)/(nTheta+pad))+1;
lutCol(:,3) = positions(1+offset2:nElem+offset2)-(lutRow(:,3)-1)*(nTheta+pad)+1;

lutW(:,1) = weigths(1:nElem);
lutW(:,2) = weigths(1+offset1:nElem+offset1);
lutW(:,3) = weigths(1+offset2:nElem+offset2);