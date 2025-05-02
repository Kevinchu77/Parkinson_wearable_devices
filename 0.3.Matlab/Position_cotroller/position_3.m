clear all
clc

% Kb=85*10^(-3);
% R=0.55;
% L=25*10^(-3);
% J=0.09;
% b=0.05;

J = 3.2284E-6;
b = 3.5077E-6;
K = 0.0274;
R = 4;
L = 2.75E-6;

hold on %gi? ?? th? lai


%%
%Tinh overshoot và timerise: MP = 5%
MP = 0.01; % Độ vượt cực đại (1% = 0.01)

% Định nghĩa phương trình tinh zeta_d
syms zeta_d
eqn = exp(-zeta_d * pi / sqrt(1 - zeta_d^2)) == MP;

% Giải phương trình
sol = vpasolve(eqn, zeta_d, [0, 1]); % Giới hạn zeta_d trong khoảng 0 đến 1
zeta_d = sol

%tinh wn theo timerise = 0.1
t_r = 0.1; % Thời gian tăng
% Khai báo phương trình
syms omega_n
eqn = t_r == (1 - 0.4167 * zeta_d + 2.917 * zeta_d^2) / omega_n;

% Giải phương trình
sol = vpasolve(eqn, omega_n, [0, inf]); % Giới hạn omega_n > 0
omega_n = sol
%%

Kp = 1;
Ki = 0;
Kd = 0;
%%
s = tf('s');
%Gp = Kb/(J*L*s^2+(J*R+L*b)*s+(R*b+Kb^2));
Gp = K/(s*((J*s+b)*(L*s+R)+K^2));
Gc = pid(Kp,Ki,Kd);
G  = (Gp*Gc);
T = feedback(G,1)
% t = 0:0.001:0.1;
%pidTuner(T,'pid')

step(T)
grid on