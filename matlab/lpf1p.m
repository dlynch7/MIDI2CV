clc;
clear;
close all;

f_pwm = 490; 
w_pwm = 2*pi*f_pwm;
range_pwm = 5; % volts
mag_pwm = (8.3e-3)/range_pwm; % volts/volts
tau = sqrt(mag_pwm^-2 - 1)/w_pwm

w_BW = 1/tau;
f_BW = w_BW/(2*pi) % Hz

lpf_1pole = tf(1,[tau,1])
[voltage, time] = step(lpf_1pole);
voltage = voltage*5;
plot(time,voltage)
xlabel('Time (s)')
ylabel('Voltage (V)')
title('1-pole LPF response to 5V step')

figure
h = bodeplot(lpf_1pole)
setoptions(h,'FreqUnits','Hz','PhaseVisible','off');

S = stepinfo(voltage,time,voltage(length(voltage)))