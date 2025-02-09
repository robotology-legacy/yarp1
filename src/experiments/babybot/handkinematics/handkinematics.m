%
% plot hand kinematics
%
%

warning('off','MATLAB:dispatcher:InexactMatch');

%% load network from disk
% load 'vq'
load 'vqNetWrist'
load 'vqNetHand'

idVector = port('create', 'vector', 0, 'mcast');
if (idVector == -1)
    disp('cannot create hand posture input port');
    return;
end
e = port('register', idVector, '/handkinematics/i', 'default');
if (e == -1)
    disp('cannot register hand posture input port');
    return;
end

idForce = port('create', 'vector', 0, 'mcast');
if (idForce == -1)
    disp('cannot create force input port');
    return;
end

e = port('register', idForce, '/handkinematics/force/i', 'default');
if (e == -1)
    disp('cannot register hand force input port');
    return;
end

idWristForce = port('create', 'vector', 0, 'udp');
if (idWristForce == -1)
    disp('cannot create wirst force input port');
    return;
end
e = port('register', idWristForce, '/handkinematics/forcewrist/i', 'default');
if (e == -1)
    disp('cannot register wrist force input port');
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

idBottle = port('create', 'bottle', 0, 'udp', 'no_buffers');
if (idBottle == -1)
    disp('cannot create behavior input port');
    return;
end
e = port('register', idBottle, '/handkinematics/behavior/i', 'default');
if (e == -1)
    disp('cannot register behavior input port');
    return;
end

idOutBottle = port('create', 'bottle', 1, 'udp');
if (idOutBottle == -1)
    disp('cannot create output port');
    return;
end
e = port('register', idOutBottle, '/handkinematics/o:bot', 'default');
if (e == -1)
    disp('cannot register output bottle port');
    return;
end

%%%% disconnect (if needed) before
porter ('/handcontrol/o:status', '!/handkinematics/i');
porter ('/repeater/o', '!/handkinematics/behavior/i');
porter ('/touch/o', '!/handkinematics/touch/i');
porter ('/handcontrol/o:force', '!/handkinematics/force/i');
porter ('/wristforce/o', '!/handkinematics/forcewrist/i');
porter ('/handkinematics/o:bot', '!/repeater/i');

%%%% connect
porter('/handcontrol/o:status', '/handkinematics/i');
porter ('/handcontrol/o:force', '/handkinematics/force/i');
porter('/repeater/o', '/handkinematics/behavior/i');
porter('/touch/o', '/handkinematics/touch/i');
porter ('/wristforce/o', '/handkinematics/forcewrist/i');
porter ('/handkinematics/o:bot', '/repeater/i');

qh = zeros(1,16);
qtouch = zeros(1,17);
qforce = zeros(1,15);
wristForce = zeros(1:6);

% loop
i = 0;
az = 45;
el = 45;
exit = 0;
freeze = 1;
filename = '';
filename2 = '';
while(~exit)

   %disp('idVector');
    [qh err1] = port('read', idVector, 0);

   %disp('idTouch');
    [qtouch err2] = port('read', idTouch, 0);
      
    if (err2 < 0)
        disp('Error reading touch sensors');
        qtouch(1:17) = 0;
    end
    
   %disp('idWristForce');
     [wristForce err4] = port('read', idWristForce, 0);
     if (err4 < 0)
         disp('Error reading wrist');
         wristForce(1:6) = 0;
     end
     % zoomV(1:6) = 10;
     % plot_vector(wristForce(1:3), wristForce(4:6), zoomV);
    
   %disp('computeHandKin');
    if ( (err1 >= 0) && (~freeze))
      computeHandKin(qh, qtouch, wristForce, az, el);
    end
    
   %disp('idForce');
    [qforce err3] = port('read', idForce, 0);
    if (err3 < 0)
        qforce(1:15) = 0;
    end
    
    %% bottle
    [bottle err3] = port('read', idBottle, 0);
    if (err3 >= 0)
   %disp('received bottle');
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
            if(strcmp(bottle{2}, 'ReachingDone'))
                disp('Reaching done !');
                % data = [qh(2:16) wristForce]';
                % ask network
                isPosture = sim(vqNetHand, qh(2:16)');
                isWeight = sim(vqNetWrist, wristForce');
                isWeight(1) = 1;    % kill weight
                
                % prepare bottle
                ret = checkGrasp(isPosture, isWeight);
                outBottle{1} = 'Motor';
                if(ret == 1)
                    disp('--> grasped !!');
                    disp('I might have the object, is it light?');
                    play_beeps(10);
                    outBottle{2} = 'HandKinGraspSuccess1';
                elseif(ret == 2)
                    disp('--> grasped !!');
                    disp('I might have the object, is it a good grasp?');
                    play_beeps(10);
                    outBottle{2} = 'HandKinGraspSuccess1';
                elseif(ret == 3)
                    disp('--> grasped !!');
                    disp('Quite sure I have the object');
                    play_beeps(10);
                    outBottle{2} = 'HandKinGraspSuccess1';
                else
                    disp('--> not grasped !!');
                    disp('Pretty sure about it');
                    play_beeps(1);
                    outBottle{2} = 'HandKinGraspFailure1';
                end
               % appendToFile(qh, qtouch, wristForce);
               port('write', idOutBottle, outBottle);
            end
            if ( (strcmp(bottle{2}, 'HandKinSavePosture')) )
                %% || (strcmp(bottle{2}, 'GraspRflxClutch')))
                savePosture(filename, qh, qtouch);
                saveTouchForce(filename2, qforce, qtouch);
            end
            if (strcmp(bottle{2}, 'HandKinSetFile'))
               filename = sprintf('%s%s', bottle{3}, '.txt');
               filename2 = sprintf('%s%s', bottle{3}, 'TF.txt');
               tmp = strcat ('Future postures will be saved to:', filename);
               disp(tmp);
               tmp = strcat ('Future touch-force couple will be saved to:', filename2);
               disp(tmp);
            end
            if(strcmp(bottle{2}, 'HandKinQuit'))
                disp('Quit message received, goodbye');
                exit = 1;
            end
        end
    end
    pause(0.1);
end

porter ('/handcontrol/o:status', '!/handkinematics/i');
port('unregister', idVector);
port('destroy', idVector);

porter ('/handcontrol/o:force', '!/handkinematics/force/i');
port('unregister', idForce);
port('destroy', idForce);

porter ('/repeater/o', '!/handkinematics/behavior/i');
port('unregister', idBottle);
port('destroy', idBottle);

porter ('/touch/o', '!/handkinematics/touch/i');
port('unregister', idTouch);
port('destroy', idTouch);

porter ('/wristforce/o', '!/handkinematics/forcewrist/i');
port('unregister', idWristForce);
port('destroy', idWristForce);

porter ('/handkinematics/o:bot', '!/repeater/i');
port('unregister', idOutBottle);
port('destroy', idOutBottle);

clear port;

quit;