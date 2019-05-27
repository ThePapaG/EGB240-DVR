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

%% Second second order design
c3 = -p(3)-p(4);
c4 = p(3)*p(4);

wn_second = sqrt(c4)
Q_second = wn_first/c3

%% Third second order design
c5 = -p(5)-p(6);
c6 = p(5)*p(6);

wn_third = sqrt(c6)
Q_third = wn_first/c5