function plotsimsom(net, data, labels)
% Simulates and plots a SOM
%   plotsimsom(net, data, labels) takes
%   net    - The SOM network.
%   data   - The data that shall be simulated and plotted.
%   labels - A cell-array or double matrix with the labels (optional).

disp('Plotting SOM');

% if double matrix then convert
if ~iscell(labels)
  labels_tmp=cell(length(labels),1);
  for i=1:length(labels)
     labels_tmp{i}=num2str(labels(i));
  end;
labels=labels_tmp;
end


% assign labels to each unit
[output dummy]=sim(net,data);
if nargin==3
  [dummy len]=size(output);
  neuron_label=cell(1,net.layers{1}.size);
  for i=1:len
    winner=find(output(:,i));
    neuron_label{winner}=strvcat(neuron_label{winner}, labels{i});
  end;
end;


% plot the labels at appropriate units
plotsom(net.layers{1}.positions)
for i=1:net.layers{1}.size
  A(i)=length(find(output(i,:)~=0));
  if A(i)~=0 
    text(net.layers{1}.positions(1,i)-0.15,net.layers{1}.positions(2,i),int2str(A(i)));
    if nargin==3
      text(net.layers{1}.positions(1,i)+0.1,net.layers{1}.positions(2,i), neuron_label{i});    
    end;	
  else
    text(net.layers{1}.positions(1,i)-0.15,net.layers{1}.positions(2,i),'none');
  end
end
whitebg(gcf,'white');
axis fill;
drawnow;
