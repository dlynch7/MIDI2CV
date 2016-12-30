clear;
clc;
close all;

R1 = 39e3;
R2 = R1;
C1 = 0.1e-6;
C2 = C1;

wo = 1/sqrt(R1*R2*C1*C2);
zeta = ((R1+R2)*sqrt(R1*R2*C1*C2))/(2*C1*R1*R2);

lpf2p = tf(wo^2,[1,2*zeta*wo,wo^2])
h = bodeplot(lpf2p);
% setoptions(h,'FreqUnits','Hz','PhaseVisible','off');
figure
step(lpf2p)
S = stepinfo(lpf2p)

wpwm = 980*2*pi;
pwmMag = 5;
rippleMag = sqrt((wo^4)/(((wpwm^2)+(wo^2))^2))
rippleMagVolts = rippleMag*pwmMag