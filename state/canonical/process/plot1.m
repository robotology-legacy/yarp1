function plot1(obj_id,show_mode)

data = load('data02.txt');
d_principal = data(:,8);
d_dx = data(:,5)-data(:,3);
d_dy = data(:,6)-data(:,4);
%%d_final_dir = atan2(d_dx,d_dy);  %% rotate 90 degrees
d_final_dir = atan2(d_dy,d_dx);
d_displacement = sqrt(d_dx.^2+d_dy.^2);
d_object = data(:,1);
d_gesture = data(:,7);

%obj_id = 2;
%show_mode = 2;

gestures = 0:3;
markers = 'ox+*';
colors = 'rgbk';
gesture_names = { 'gesture 0 (back-slap)', 'gesture 1 (draw-in)', ...
		  'gesture 2 (side-tap)', 'gesture 3 (other back-slap)' };

%figure(1);
%clf;

handles = [];
names = {};

for i=1:4
%%  idx = find((d_object==obj_id) & (d_gesture==gestures(i)));
idx = find((d_gesture==gestures(i)));
  hold on;
  if show_mode == 0
    h = scatter((180/pi)*d_principal(idx), (180/pi)*d_final_dir(idx), ...
		[colors(i) markers(i)]);
    axis([-90 90 -180 180]);
  elseif show_mode == 1
    h = scatter((180/pi)*d_principal(idx), d_displacement(idx), ...
		[colors(i) markers(i)]);
    axis([-90 90 0 60]);
  elseif show_mode == 2
    v1 = d_final_dir(idx);  x1 = [cos(v1), sin(v1)];
    v2 = d_principal(idx);  x2 = [cos(v2), sin(v2)];
    theta1 = acos(sum(x1.*x2,2));
    theta2 = acos(-sum(x1.*x2,2));
    diff = min(abs(theta1),abs(theta2));
    h = scatter((180/pi)*d_principal(idx), ...
		(180/pi)*diff, ...
		[colors(i) markers(i)]);
    axis([-90 90 0 90]);
    [v1 v2 diff]*180/pi
  end;
  if (length(h)>0)
    % dummy line, just to simplify showing legend
    h = plot([0 0], [0 0], [colors(i) markers(i)]);
    set(h,'visible','off');
    handles = [handles h(1)];
    names = {names{:} gesture_names{i}};
  end;
  hold off;
end;

xlabel('Principal axis of object');
if show_mode == 0
  ylabel('Direction of motion');
elseif show_mode == 1
  ylabel('Total displacement');
elseif show_mode == 2
  ylabel('Diff in motion dir & principal axis');
end;
title(['object number ' num2str(obj_id)]);

legend(handles,names);
