
headPort = port('create', 'vector', 0, 'mcast');

if (headPort == -1)
    disp('cannot create head position port');
    return;
end

e = port('register', headPort, '/3dtest/head/i:vect', 'default');
if (e == -1)
    disp('cannot register head posture input port');
    return;
end

fixationPort = port('create', 'bottle', 0, 'udp');

if (fixationPort == -1)
    disp('cannot create fixation port');
    return;
end

e = port('register', fixationPort, '/3dtest/fixation/i:bot', 'default');
if (e == -1)
    disp('cannot register fixation input port');
    return;
end

%% connect
porter ('/headcontrol/position/o', '!/3dtest/head/i:vect');
porter ('/checkfixation/bottle/out', '!/3dtest/fixation/i:bot');

porter ('/headcontrol/position/o', '/3dtest/head/i:vect');
porter ('/checkfixation/bottle/out', '/3dtest/fixation/i:bot');

%%%%%%%%%%% LOOP
[head err3] = port('read', headPort, 1);
ref = headkinematics(head);

exit = false;
N = 0;
figure(1), hold on;
while(~exit)
    [bottle err3] = port('read', fixationPort, 1);
    if(strcmp(bottle{2}, 'SaccadeFixated'))
       disp('fixation has been achieved');
       N = N+1;
    else
        exit = true;
    end
    if (N == 10)
        exit = true;
    end
    
    [head err3] = port('read', headPort, 1);
    fix = headkinematics(head);
    delta = fix-ref;
    figure(1), scatter3(delta(1), delta(2), delta(3), 'x'), hold on;
end

figure(1), hold off;
     


%%%%%%%%%%%%%%%%%%%

%% disconnect
porter ('/headcontrol/position/o', '!/3dtest/head/i:vect');
porter ('/checkfixation/bottle/out', '!/3dtest/fixation/i:bot');

port('unregister', fixationPort);
port('destroy', fixationPort);

port('unregister', headPort);
port('destroy', headPort);

clear port;