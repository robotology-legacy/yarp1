portID=port('create', 'bottle', 0, 'tcp');

if (portID==-1)
    disp('cannot create input port');
    return;
else
    disp('port was created ok');
end  

e=port('register', portID, '/matlab/i', 'default');
if (e==-1)
    disp('cannot register input port');
    return;
else
    disp('input port was registered');
end

portID2=port('create', 'bottle', 1, 'tcp');

if (portID2==-1)
    disp('cannot create output port');
    return;
else
    disp('output port was created ok');
end  

e=port('register', portID2, '/matlab/o', 'default');
if (e==-1)
    disp('cannot register output port');
    return;
else
    disp('output port was registered');
end

exit=0;
count=30;

while(~exit)
    [val err1]=port('read', portID,0);
    
    if (err1>=0)
        disp('YUP, RECEIVED SOME DATA');
        val
        disp('Writing it back to output port');
        outbot=val;
        outbot{1}='Hello from MATLAB';
        port('write', portID2, outbot);
    else
        disp('WAITING FOR SOME DATA');
    end

    count=count-1;
    if(count==0)
        exit=1;
    end
    
    pause(0.5)
end

disp('I am tired now, have a nice evening');

port('unregister', portID);
port('destroy', portID);

port('unregister', portID2);
port('destroy', portID2);

clear port;
    