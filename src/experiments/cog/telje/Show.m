
data = load('pts1.txt');


r = data(:,1);
x = data(:,2);
y = data(:,3);

figure(1);
clf;
scatter(data(:,2),data(:,3),'w');
hold on;
for i=1:length(x)
  [x(i) y(i) r(i)]
  text(x(i),y(i),num2str(r(i)));
end;
hold off;
