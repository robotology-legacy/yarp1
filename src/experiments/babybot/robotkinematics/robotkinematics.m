%
% plot hand kinematics
%
%

idVector = port('create', 'vector', 0, 'mcast');
if (idVector == -1)
    disp('cannot create head posture input port');
    return;
end

e = port('register', idVector, '/handkinematics/i', 'default');
if (e == -1)
    disp('cannot register hand posture input port');
    return;
end

idTouch = port('create', 'vector', 0, 'mcast');
if (idTouch == -1)
    disp('cannot create touch port');
    return;
end
e = port('register', idTouch, '/handkinematics/touch/i', 'default');
if (e == -1)
    disp('cannot register touch input port');
    return;
end

idBottle = port('create', 'bottle', 0, 'udp');
if (idBottle == -1)
    disp('cannot create behavior input port');
    return;
end
e = port('register', idBottle, '/handkinematics/behavior/i', 'default');
if (e == -1)
    disp('cannot register behavior input port');
    return;
end

%%%% disconnect (if needed) before
porter ('/handcontrol/o:status', '!/handkinematics/i');
porter ('/repeater/o', '!/handkinematics/behavior/i');
porter ('/touch/o', '!/handkinematics/touch/i');


%%%% connect
porter('/handcontrol/o:status', '/handkinematics/i');
porter('/repeater/o', '/handkinematics/behavior/i');
porter('/touch/o', '/handkinematics/touch/i');

qh = zeros(1,16);
qtouch = zeros(1,17);

% loop
i = 0;
az = 45;
el = 45;
exit = 0;
freeze = 0;
filename = '';
while(~exit)
    [qh err1] = port('read', idVector, 0);
    [qtouch err2] = port('read', idTouch, 0);
    if (err2 < 0)
        qtouch(1:17) = 0;
    end
    if ( (err1 >= 0) && (~freeze))
      computeHandKin(qh, qtouch, az, el);
    end
    
    %% bottle
    [bottle err3] = port('read', idBottle, 0);
    if (err3 >= 0)
        % received a bottle
        if (strcmp(bottle{1},'Motor'))
            if(strcmp(bottle{2}, 'IsAlive'))
                disp('I''m alive!');
            end
            if(strcmp(bottle{2}, 'HandKinView'))
                disp('Changing viewpoint');
                az = bottle{3}(1);
                el = bottle{3}(2);
            end
            if(strcmp(bottle{2}, 'HandKinFreeze'))
                if (freeze == 1)
                    disp('Resuming display');
                    freeze = 0;
                else
                    disp('Freezing display');
                    freeze = 1;
                end
            end
            if ( (strcmp(bottle{2}, 'HandKinSavePosture')) || (strcmp(bottle{2}, 'GraspRflxClutch')))
                savePosture(filename, qh, qtouch);
            end
            if (strcmp(bottle{2}, 'HandKinSetFile'))
               filename = bottle{3};
               tmp = strcat ('Future posture will be saved to:', filename);
               disp(tmp);
            end
            if(strcmp(bottle{2}, 'HandKinQuit'))
                disp('Quit message received, goodbye');
                exit = 1;
            end
        end
    end
    pause(0.5);
end

porter ('/handcontrol/o:status', '!/handkinematics/i');
port('unregister', idVector);
port('destroy', idVector);

porter ('/repeater/o', '!/handkinematics/behavior/i');
port('unregister', idBottle);
port('destroy', idBottle);

porter ('/touch/o', '!/handkinematics/touch/i');
port('unregister', idTouch);
port('destroy', idTouch);

clear port;

