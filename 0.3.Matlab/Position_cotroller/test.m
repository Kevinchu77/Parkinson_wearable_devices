% Tham số ẩn
syms s K Kp J b L R;
Ki = 0;
Kd = 0;

% Hàm truyền hệ thống cơ bản (Gp)
Gp = K / (s * ((J * s + b) * (L * s + R) + K^2));

% Bộ điều khiển PID (Gc)
Gc = Kp + Ki / s + Kd * s;

% Hàm truyền vòng mở (G)
G = Gp * Gc;

% Hàm truyền vòng kín (T)
T = simplify(G / (1 + G))


% Hiển thị kết quả
disp('Hàm truyền vòng kín T:');
disp(T);
