
data = load('data02.txt');
d_principal = data(:,8);
d_dx = data(:,5)-data(:,3);
d_dy = data(:,6)-data(:,4);
d_final_dir = atan2(d_dy,d_dx);
d_object = data(:,1);
d_gesture = data(:,7);

obj_id = 2;

gestures = 1:4;
markers = 'ox+*';
colors = 'rgbk';
gesture_names = { 'gesture 1 (back-slap?)', 'gesture 2 (side-tap?)', ...
		  'gesture 3 (draw-in?)', 'gesture 4 (other back-slap?)' };

figure(2);
clf;

handles = [];
names = {};

for i=1:4
  idx = find((d_object==id_car) & (d_gesture==gestures(i)));
  hold on;
  h = scatter((180/pi)*d_principal(idx), ...
	      (180/pi)*d_final_dir(idx), ...
	      [colors(i) markers(i)]);
  if (length(h)>0)
    % dummy line, just to simplify showing legend
    h = plot([5000 6000], [5000 6000], [colors(i) markers(i)]);
    handles = [handles h(1)];
    names = {names{:} gesture_names{i}};
  end;
  hold off;
  axis([-90 90 -180 180]);
end;

xlabel('Principal axis of object, as presented');
ylabel('Direction of motion');
title(['Some information about object number ' num2str(obj_id)]);

handles 
names
legend(handles,names);
