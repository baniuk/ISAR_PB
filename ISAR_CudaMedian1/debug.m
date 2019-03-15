a = 1:40*20;
a = reshape(a,40,20)';
savebinarymatrix(a,'testmatrix.dat');
%% 2
a = round(100*abs(rand(2000,4000)));
savebinarymatrix(a,'testmatrix.dat');
%% real
img = double(imread('T_14-32-53_836A.tif'));
savebinarymatrix(img,'testmatrix.dat');
%% kontrola
ImportDumpFile('debug.out');
% wynik
%% szybki MSE
in = testmatrix;
out = wynik;

err = sqrt(sum( ( in(:) - out(:) ).^2)/length(in))
%% ile zajmuje w pami�ci

okno = [51 51];
imag = [2000 4000];
ratio = imag(2)/imag(1);
grid_w = 100;

grid = [floor(grid_w/ratio) grid_w];
mem_okno = prod(okno)*4/1024;   % w KB
mem_total = prod(grid)*mem_okno;

sprintf('Okno %d [KB]    Total %d [KB]\n',mem_okno, mem_total)