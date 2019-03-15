ImportDumpFile('debug.out')
plot(out_linspace)
out_linspace([1 100]) % powinno byc [0 10]
out_linspace1 % powinno byc 0 10
out_linspace2 % powinno byc 10

[L1a L1b L1pion] % pozioma pomiedzy (0,0) i (10,0)
[L2a L2b L2pion] % pionowa (10,0) (10,10)
[L3a L3b L3pion] % uko�na (0,0) (10,10)

% odcinek (0,0) i (10,0) 10 punkt�w
XY1 = [S1_X;S1_Y]'
plot(XY1(:,1),XY1(:,2))

% odcinek (0,0) i (10,10) 20 punkt�w
XY3 = [S3_X;S3_Y]'
plot(XY3(:,1),XY3(:,2),'o')

% punkt przeci�cia  L0 (0,0)(10,0)
%                   L1 (10,0)(10,10)
[P1cut_X;P1cut_Y]
% punkt przeci�cia  L0 (0,0)(10,0)
%                   L1 (0,0)(10,10)
[P2cut_X;P2cut_Y]

P_OUT = [P_OUT_X;P_OUT_Y]';
plot(P_OUT(:,1),P_OUT(:,2),'o')

figure
P1_OUT = [P1_OUT_X;P1_OUT_Y]';
plot(P1_OUT(:,1),P1_OUT(:,2),'o')
axis square

% test mexa
addpath ..\x64\debug

V = [0 0;
     10 0;
     15 10;
     5 10];
mOut = mexFillPoly(V(:,1),V(:,2),30);
figure
plot(mOut(1,:),mOut(2,:),'o')
clear mexFillPoly