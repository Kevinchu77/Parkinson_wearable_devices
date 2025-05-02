% Tham số của hệ thống
Kp = 100;
numerator = 0.0274*Kp; % Tử số = 1 (theo phương trình đã cho)
denominator = [8.878e-12, 1.291e-05, 0.0007648, 2.74]; % Đa thức bậc 3 trong mẫu

% Tạo hàm truyền
G = tf(numerator, denominator);

% Tính các cực của hệ thống
poles = pole(G);

% Hiển thị các cực
disp('Các cực của hệ thống:');
disp(poles);

% Vẽ biểu đồ cực
figure;
pzmap(G);  % Vẽ đồ thị cực của hệ thống
title('Đồ thị cực của hệ thống bậc 3');
grid on;
