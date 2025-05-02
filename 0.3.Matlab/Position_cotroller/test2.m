% Tham số đã cho
%Kp = sym('Kp');  % Kp là ẩn
Kp = 100;
J = 3.2284E-6;
b = 3.5077E-6;
K = 0.0274;
R = 4;
L = 2.75E-6;
s = sym('s');  % Biến Laplace

% Tử số
numerator = K * Kp;

% Mẫu số
denominator = (K^2 * s + K * Kp + R * b * s + J * L * s^3 + J * R * s^2 + L * b * s^2);

% Hàm truyền
H = numerator / denominator;

% Hiển thị kết quả
disp('Hàm truyền sau khi thay giá trị:');
disp(H);