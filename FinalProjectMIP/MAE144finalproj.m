clear all 
close all

%MIP PROPERTIES
s=tf('s');
g=9.81;     %Gravity
m_b=0.180;  %body mass
m_w=0.027;  %wheel mass
r=0.034;    %radius of wheel
wf = 1760;  %freerun speed
l=0.0477;   %distance to wheel axis
tau_s=0.003;%stall torque
Gr=35.55;    %gearbox ratio
V_n=7.4;     %nominal battery
I_b=0.000263; %mips total moment of inertia
I_m=3.6*10^-8; %motor armature moment of inertia
I_w=2*((m_w*r^2)/2+Gr^2*I_m); %wheel's moment of inertia accounting for gearbox
k=tau_s/wf; %motor constant

%desired characteristics of our final system:

innerh=0.01; %refresh rate of inner loop
outerh=0.05; %refresh rate of outer loop
wnyq=pi/innerh; %nyquist freq. of our system
wc1=wnyq/10; %crossover of outer loop
wc2=wc1/10; %crossover of inner loop

%Calculate coefficents for plant equations
a=I_w+(m_b+m_w)*r^2;
b=m_b*r*l;
c=I_b+m_b*l^2;
d=m_b*g*l;
e=2*Gr*tau_s;
f=2*k*Gr^2;

%%
%%%%%%%%INNER LOOP STUFF%%%%%%%%%%%

G1=((a+b)*e*s)/((b^2-a*c)*s^3-(a+2*b+c)*f*s^2+a*d*s+d*f);
g1poles=pole(G1);

% %2nd order pade approx for delay induced by DAC
% [delaynum,delaydenom]=pade(innerh/2,2);
% padedelay=tf(delaynum,delaydenom);
% G1=minreal(G1*padedelay);

K1=-2.55; %inner loop gain

D1lag=(s-g1poles(3))/s; %lag compensator to cancel zero at origin and pole
%at ~-5.6

%used to design a lead controller centered around crossover
ratio1 = 1.5; %2.5
z1 = wc1/sqrt(ratio1);
p1 = wc1*sqrt(ratio1);
D1lead = (s+z1)/(s+p1);

D1=K1*D1lag*D1lead; %Combine parts into one controller

%Discretize D1 using tustin's approx w/ prewarping around wc1
discopt1 = c2dOptions('Method','tustin','PrewarpFrequency',wc1);
discD1 = c2d(D1,innerh,discopt1);


%make open and closed loop TFs of inner loop
OL1=minreal(G1*D1);
CL1=feedback(OL1,1);
innerPF=1/dcgain(CL1); %prefactor to compensate for any steadystate error.

%plot inner loop
figure('Name','inner loop');
subplot(3,1,1);
rlocus(OL1);

subplot(3,1,2);
hold on
bode(OL1);
margin(OL1);

subplot(3,1,3);
step(innerPF*CL1);
%%
%%%%%%%%OUTER LOOP STUFF%%%%%%%%%%%
G2=(-(b+c)*s^2+d)/((a+b)*s^2);

K2=.2175;

%design outer loop lead compensator around wc2
ratio2 = 10;
z2 = wc2/sqrt(ratio2);
p2 = wc2*sqrt(ratio2);
D2lead=(s+z2)/(s+p2);

D2lag=1;%(s+2)/(s+.0);%/s; 


D2=K2*D2lead*D2lag;
OL2=minreal(G2*D2);
CL2=feedback(OL2,1);

%Discretize D2 using tustin's approx w/ prewarping around wc2
discopt2 = c2dOptions('Method','tustin','PrewarpFrequency',wc2);
discD2 = c2d(D2,outerh,discopt2);

%plot outer loop
figure('Name','outerloop');
subplot(2,2,1);
rlocus(OL2);


subplot(2,2,2);
hold on
bode(OL2);
margin(OL2);

subplot(2,2,3);
step(CL2);
%%

fullopen=minreal(D2*innerPF*CL1*G2);
fullclosed=feedback(fullopen,1);

%plot outer loop step response with inner loop inside 
figure('Name', 'both loops');
subplot(2,2,1);
rlocus(OL2);
axis([-20 20 -2 2]);
title('Root Locus of Outer Loop Only');

subplot(2,2,2);
hold on
bode(fullopen);
title('Bode Plot of Total Open Loop System');
margin(fullopen);

subplot(2,2,3);
step(CL2);
title('Step Response of Outer Loop Only');

subplot(2,2,4);
step(fullclosed);
title('Step Response of Full System');
