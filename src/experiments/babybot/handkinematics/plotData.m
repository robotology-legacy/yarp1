function plotData(P, nL, c)

[X Y Z] = separa (P, nL);

figure(1), scatter3(X,Y,Z,c);

function [X, Y, Z] = separa(P, nL)

xx = 1;
yy = 1;

for p = 1:size(P,2)
    for n = 1:size(P,1)
        if (P(n,p) == 1)
            X(p) = xx;
            Y(p) = yy;
            Z(p) = 1;
        end
        xx = xx + 1;
        if (xx > nL(1))
            xx = 1;
            yy = yy + 1;
            if (yy > nL(2))
                yy = 1;
            end
        end
    end
end

    







