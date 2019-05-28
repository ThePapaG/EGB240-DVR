close all; clear all; clc;

%% Design
fsamp = 15.625e3;
fs = fsamp/2;
fp = 4000;

wp = 2*pi*fp;
ws = 2*pi*fs;

Amin = 20*log10(1/2^8);
Amax = 1;

[n, wn] = cheb1ord(wp, ws, Amax, Amin, 's');
[b, a] = cheby1(n, Amax, wn, 'low', 's');
G = tf(b,a);
%bode(G);

[z,p,k] = tf2zpk(b,a);

%% First second order design
c1= -p(1)-p(2);
c2 = p(1)*p(2);

wn_first = sqrt(c2)
Q_first = wn_first/c1

C1A = 1e-9;
C2A = 18e-9;
RA = 2.7e3;
K = 1;

NumA = [0 0 K/(RA*RA*C1A*C2A)];
denA = [1 (1/(RA*C1A))+(1/(RA*C2A))+((1-K)/(RA*RA*C1A*C2A)) 1/(RA*RA*C1A*C2A)];
tfA = tf(NumA, denA);

%% Second second order design
c3 = -p(3)-p(4);
c4 = p(3)*p(4);

wn_second = sqrt(c4)
Q_second = wn_first/c3

C1B = 560e-12;
C2B = 22e-9;
RB = 16e3;
K = 1;

NumB = [0 0 K/(RB*RB*C1B*C2B)];
denB = [1 (1/(RB*C1B))+(1/(RB*C2B))+((1-K)/(RB*RB*C1B*C2B)) 1/(RB*RB*C1B*C2B)];
tfB = tf(NumB, denB);

%% Third second order design
c5 = -p(5)-p(6);
c6 = p(5)*p(6);

wn_third = sqrt(c6)
Q_third = wn_first/c5

C1C = 120e-12;
C2C = 33e-9;
RC = 20e3;
K = 1;

NumC = [0 0 K/(RC*RC*C1C*C2C)];
denC = [1 (1/(RC*C1C))+(1/(RC*C2C))+((1-K)/(RC*RC*C1C*C2C)) 1/(RC*RC*C1C*C2C)];
tfC = tf(NumC, denC);

%% Plot


tffinal = tfA * tfB * tfC;
stability = isstable(tffinal)
figure (2);
h = bodeplot(tffinal); % display bode plot
setoptions(h,'FreqUnits','Hz'); % change units to Hz
setoptions(h,'FreqUnits','Hz','PhaseVisible','off'); % change units to Hz
setoptions(h,'Xlim',[10,10e3]);
 setoptions(h,'Ylim',[-27,25]);
 grid on;
 hold on;
 bodeplot(tfA);
 bodeplot(tfB);
 bodeplot(tfC);
 title({'Bode Diagram','Component Values'});
 legend('Overall', 'Stage A', 'Stage B', 'Stage C', 'Location','southwest');