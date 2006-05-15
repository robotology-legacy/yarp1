function [NumberOfSignals, NameOfSignals, Signals] = ReadJamesTxt(FileName, Delimiter)

%Genova 03/08/2005
%Edited by Francesco Nori
%
% This function reads a .txt file containing
% the data sent by the DSP. The first row of
% the given .txt is assumed to contain the
% names of the read variables. Columns are assumed
% to be separated by spaces.

fid = fopen(FileName);
FirstLine = fgetl(fid);
NameOfSignals = strread(FirstLine, '%s','delimiter', Delimiter);

Line = FirstLine;
Signals = [];
NumberOfSignals = 0;
Line = fgetl(fid);
while Line ~=-1
    NumberOfSignals = NumberOfSignals + 1;
    Row = strread(Line, '%f','delimiter', Delimiter);
    Signals = cat(1, Signals, Row');
    Line = fgetl(fid);
end