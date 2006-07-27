function ret = checkGrasp(isPosture, isWeight)

if(isWeight(1) == 0)
    % ok weight tells me I have it
    if (isPosture(1) == 0)
        ret = 3;    % quite sure it is a good grasp
    else
        ret = 2;    % ok probably not a good grasp
    end
elseif (isPosture(1) == 0)
    % of the object is probably light
    ret = 1;        % ok it is probably a light object
else
    ret = 0;
end 