a = [1 1 1 1 0 0 0 0];
a = repmat(a,400,100);
savebinarymatrix(a,'moon.dat');

%% kontrola
ImportDumpFile('debug.out');
data = reshape(data,800,[])';
figure;imshow(data,[]);
hold on
plot(tabp_x(1,:),tabp_y(1,:),'.');
plot(tabp_x(2,:),tabp_y(2,:),'.');
plot(tabp_x(3,:),tabp_y(3,:),'.');



x = 1:800;
plot(x,l2_a*x+l2_b,'-r')
plot(x,l3_a*x+l3_b,'-r')
plot(P0L2_x,P0L2_y,'bs')
plot(P1L2_x,P1L2_y,'bs')
plot(P0L3_x,P0L3_y,'bs')
plot(P1L3_x,P1L3_y,'bs')



figure;plot(line','-o');grid
figure;plot(mean','-o');grid