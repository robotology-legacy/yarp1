function imgCart = logpolar2Cart(logpolar)
% remap logpolar image to cartesian space
% by nat Apr. 2004
% Note: image input is assumed to be a raw logpolar image (uint8) 152x252x1
% Output is uint8 256x256x3 (RGB)

% Set your own yarp_root folder
YARP_ROOT = 'D:/nat/yarp/';
fileName = 'conf/RemapMap_0.235_256x256_P8.gio';

xSize = 256;
ySize = 256;

% read lut from file
[lutR, lutC] = readLut(sprintf('%s%s', YARP_ROOT, fileName));

% remap logpolar image to cartesian space
imgLpColored = reconstructColor(logpolar);
imgCart = uint8(zeros(xSize, ySize, 3));
i = 1;
for r = 1:ySize
    for c = 1:xSize
        imgCart(r,c,:) =  imgLpColored(lutR(i), lutC(i), :);
        i = i+1;
    end
end

function [lutRow, lutCol] = readLut(filename)
%% read map from file
xSize = 256;
ySize = 256;
nRho = 152;
nTheta = 252;
pixNumb = 2;

%% read raw lut from file
fid = fopen(filename, 'r', 'l');
lutSize = xSize*ySize;
[remap n]= fread(fid, lutSize, 'int');
fclose(fid);

%%%%% build LUTs
pad = 4;
M = nTheta*3;
N = nRho*3;
lutRow = floor((remap)/(M+pad))+1;
lutCol = (remap-(lutRow-1)*(M+pad))/3+1;