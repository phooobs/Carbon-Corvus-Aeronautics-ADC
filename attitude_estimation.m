clear all
close all
clc

M = readmatrix('data/0_15_-15_fiberglass_4_18_2021_a');

%M = M(1:40000,:);
%startD = 6000;
%endD = 8000; 
startD = 1;
endD = 40000; 
M = M(startD:endD,:);

t = M(:,1);

% Avg. time step
dt = (t(length(t)) - t(1)) / length(t);

Ax = zeros(length(t), 3);
Ay = zeros(length(t), 3);
Az = zeros(length(t), 3);
Gx = zeros(length(t), 3);
Gy = zeros(length(t), 3);
Gz = zeros(length(t), 3);

phi_hat_kalman    = zeros(length(t), 3);
bias_phi_kalman   = zeros(length(t), 3);
theta_hat_kalman  = zeros(length(t), 3);
bias_theta_kalman = zeros(length(t), 3);

for j = 0:2

    Ax(:,j + 1) = M(:,5+j*6);
    Ay(:,j + 1) = M(:,6+j*6);
    Az(:,j + 1) = M(:,7+j*6);
    Gx(:,j + 1) = M(:,8+j*6);
    Gy(:,j + 1) = M(:,9+j*6);
    Gz(:,j + 1) = M(:,10+j*6);
    
    % low pass filter
    %cuttoffFrequency = 1;
    %Ax(:,j + 1) = lowpass(Ax(:,j + 1), cuttoffFrequency, 1/dt);
    %Ay(:,j + 1) = lowpass(Ay(:,j + 1), cuttoffFrequency, 1/dt);
    %Az(:,j + 1) = lowpass(Az(:,j + 1), cuttoffFrequency, 1/dt);
    %Gx(:,j + 1) = lowpass(Gx(:,j + 1), cuttoffFrequency, 1/dt);
    %Gy(:,j + 1) = lowpass(Gy(:,j + 1), cuttoffFrequency, 1/dt);
    %Gz(:,j + 1) = lowpass(Gz(:,j + 1), cuttoffFrequency, 1/dt);
    
    % high pass filter
    %cuttoffFrequency = 1;
    %Ax(:,j + 1) = highpass(Ax(:,j + 1), cuttoffFrequency, 1/dt);
    %Ay(:,j + 1) = highpass(Ay(:,j + 1), cuttoffFrequency, 1/dt);
    %Az(:,j + 1) = highpass(Az(:,j + 1), cuttoffFrequency, 1/dt);
    %Gx(:,j + 1) = highpass(Gx(:,j + 1), cuttoffFrequency, 1/dt);
    %Gy(:,j + 1) = highpass(Gy(:,j + 1), cuttoffFrequency, 1/dt);
    %Gz(:,j + 1) = highpass(Gz(:,j + 1), cuttoffFrequency, 1/dt);

    % Convert gyroscope measurements to radians
    Gx_rad = Gx(:,j + 1) * pi / 180.0;
    Gy_rad = Gy(:,j + 1) * pi / 180.0;
    Gz_rad = Gz(:,j + 1) * pi / 180.0;

    % 1) Accelerometer only
    phi_hat_acc   = atan2(Ay(:,j + 1), sqrt(Ax(:,j + 1) .^ 2 + Az(:,j + 1) .^ 2));
    theta_hat_acc = atan2(-Ax(:,j + 1), sqrt(Ay(:,j + 1) .^ 2 + Az(:,j + 1) .^ 2));

    % 2) Gyroscope only
    phi_hat_gyr   = zeros(length(t), 1);
    theta_hat_gyr = zeros(length(t), 1);

    for i = 2:length(t)
       p = Gx_rad(i);
       q = Gy_rad(i);
       r = Gz_rad(i);

       phi_hat   = phi_hat_gyr(i - 1);
       theta_hat = theta_hat_gyr(i - 1);

       phi_hat_gyr(i)   = phi_hat   + dt * (p + sin(phi_hat) * tan(theta_hat) * q + cos(phi_hat) * tan(theta_hat) * r);
       theta_hat_gyr(i) = theta_hat + dt * (cos(phi_hat) * q - sin(phi_hat) * r);
    end

    % 4) Kalman Filter
    A = [1 -dt 0 0; 0 1 0 0; 0 0 1 -dt; 0 0 0 1];
    B = [dt 0 0 0; 0 0 dt 0]';
    C = [1 0 0 0; 0 0 1 0];
    P = eye(4);
    Q = eye(4) * 0.01;
    R = eye(2) * 10;
    state_estimate = [0 0 0 0]';

    for i=2:length(t)
        p = Gx_rad(i);
        q = Gy_rad(i);
        r = Gz_rad(i);

        phi_hat   = phi_hat_kalman(i - 1, j + 1);
        theta_hat = theta_hat_kalman(i - 1, j + 1);

        phi_dot   = p + sin(phi_hat) * tan(theta_hat) * q + cos(phi_hat) * tan(theta_hat) * r;
        theta_dot = cos(phi_hat) * q - sin(phi_hat) * r;

        % Predict
        state_estimate = A * state_estimate + B * [phi_dot, theta_dot]';
        P = A * P * A' + Q;

        % Update
        measurement = [phi_hat_acc(i) theta_hat_acc(i)]';
        y_tilde = measurement - C * state_estimate;
        S = R + C * P * C';
        K = P * C' * (S^-1);
        state_estimate = state_estimate + K * y_tilde;
        P = (eye(4) - K * C) * P;

        phi_hat_kalman(i, j + 1)    = state_estimate(1);
        bias_phi_kalman(i, j + 1)   = state_estimate(2);
        theta_hat_kalman(i, j + 1)  = state_estimate(3);
        bias_theta_kalman(i, j + 1) = state_estimate(4);
    end

    % Convert all estimates to degrees
    phi_hat_acc = phi_hat_acc * 180.0 / pi;
    theta_hat_acc = theta_hat_acc * 180.0 / pi;
    
    phi_hat_gyr = phi_hat_gyr * 180.0 / pi;
    theta_hat_gyr = theta_hat_gyr * 180.0 / pi;
    
    phi_hat_kalman(:, j + 1) = phi_hat_kalman(:, j + 1) * 180.0 / pi;
    theta_hat_kalman(:, j + 1) = theta_hat_kalman(:, j + 1) * 180.0 / pi;
    
    if (j == 1) % rotate body only for 4-18-2020
        % pitch = roll
        % roll = -pitch
        temp = phi_hat_kalman(:, 2);
        phi_hat_kalman(:, 2) = -theta_hat_kalman(:, 2);
        theta_hat_kalman(:, 2) = temp;
    end
    
    % low pass filter
    %cuttoffFrequency = 0.001;
    %phi_hat_kalman(:,j + 1) = lowpass(phi_hat_kalman(:,j + 1), cuttoffFrequency, 1/dt);
    %theta_hat_kalman(:,j + 1) = lowpass(theta_hat_kalman(:,j + 1), cuttoffFrequency, 1/dt);
    
    % high pass filter
    %cuttoffFrequency = 10;
    %phi_hat_kalman(:,j + 1) = highpass(phi_hat_kalman(:,j + 1), cuttoffFrequency, 1/dt);
    %theta_hat_kalman(:,j + 1) = highpass(theta_hat_kalman(:,j + 1), cuttoffFrequency, 1/dt);

    % Plots
    figure(1)
    subplot(3, 2, 2*j+1);
    plot(t, phi_hat_kalman(:, j + 1))
    %plot( t, phi_hat_acc, t, phi_hat_gyr, t, phi_hat_kalman)
    %legend('Accelerometer', 'Gyro', 'Kalman')
    xlabel('Time (s)')
    ylabel('Angle (Degrees)')
    title('Roll')

    subplot(3, 2, 2*j+2);
    plot(t, theta_hat_kalman(:, j + 1))
    %plot(t, theta_hat_acc, t, theta_hat_gyr, t, theta_hat_kalman)
    %legend('Accelerometer', 'Gyro', 'Kalman')
    xlabel('Time (s)')
    ylabel('Angle (Degrees)')
    title('Pitch')
end

% Plot bend vs A0A
figure(2)
subplot(1, 2, 1);
title('Left')
bend = phi_hat_kalman(:, 1) - phi_hat_kalman(:, 2);
twist = theta_hat_kalman(:, 1) - theta_hat_kalman(:, 2);
surface([bend, bend],...
    [twist, twist],...
    [(1:length(t))' + startD, (1:length(t))' + startD],...
    [t, t],...
	'FaceColor', 'no',...
	'EdgeColor', 'interp');
xlabel('Bend (Degrees)')
ylabel('Twist AoA (Degrees)')
hold on
[p, S] = polyfit(bend, twist, 1);
x = -40:30;
[y, delta] = polyval(p, x, S);
plot(x, y, 'r');
plot(x, y + 2 * delta, 'r--', x, y - 2 * delta, 'r--');
grid on
axis([-40 30 -30 30])
subplot(1, 2, 2);
title('Right')
bend = phi_hat_kalman(:, 3) - phi_hat_kalman(:, 2);
twist = theta_hat_kalman(:, 3) - theta_hat_kalman(:, 2);
surface([bend, bend],...
    [twist, twist],...
    [(1:length(t))' + startD, (1:length(t))' + startD],...
    [t, t],...
	'FaceColor', 'no',...
	'EdgeColor', 'interp');
xlabel('Bend (Degrees)')
ylabel('Twist AoA (Degrees)')
hold on
[p, S] = polyfit(bend, twist, 1);
[y, delta] = polyval(p, x, S);
plot(x, y, 'r');
plot(x, y + 2 * delta, 'r--', x, y - 2 * delta, 'r--');
grid on
axis([-40 30 -30 30])

% both
%bend = cat(1, phi_hat_kalman(:, 1) - phi_hat_kalman(:, 2), phi_hat_kalman(:, 3) - phi_hat_kalman(:, 2));
%twist = cat(1, theta_hat_kalman(:, 1) - theta_hat_kalman(:, 2), theta_hat_kalman(:, 3) - theta_hat_kalman(:, 2));

%[p, S] = polyfit(bend, twist, 1);
%[y, delta] = polyval(p, x, S);
%plot(x, y, 'r');
%plot(x, y + 2 * delta, 'r--', x, y - 2 * delta, 'r--');
hold off


% Plot heatmap:
[xG, yG] = meshgrid(-5:5);
sigma = 2.5;
g = exp(-xG.^2./(2.*sigma.^2)-yG.^2./(2.*sigma.^2));
g = g./sum(g(:));
xpts = linspace(-40, 30, 500);
ypts = linspace(-30, 30, 500);

figure(3)
subplot(1, 2, 1);
title('Left')

bend = phi_hat_kalman(:, 1) - phi_hat_kalman(:, 2);
twist = theta_hat_kalman(:, 1) - theta_hat_kalman(:, 2);
N = min(histcounts2(twist(:), bend(:), ypts, xpts) * 10, 60);
imagesc(xpts, ypts, conv2(N, g, 'same'));
set(gca, 'XLim', xpts([1 end]), 'YLim', ypts([1 end]), 'YDir', 'normal');
hold on
[p, S] = polyfit(bend, twist, 1);
[y, delta] = polyval(p, x, S);
plot(x, y, 'r');
plot(x, y + 2 * delta, 'r--', x, y - 2 * delta, 'r--');
xlabel('Bend (Degrees)')
ylabel('Twist AoA (Degrees)')
hold off

subplot(1, 2, 2);
title('Right')

bend = phi_hat_kalman(:, 3) - phi_hat_kalman(:, 2);
twist = theta_hat_kalman(:, 3) - theta_hat_kalman(:, 2);
N = min(histcounts2(twist(:), bend(:), ypts, xpts) * 10, 60);
imagesc(xpts, ypts, conv2(N, g, 'same'));
set(gca, 'XLim', xpts([1 end]), 'YLim', ypts([1 end]), 'YDir', 'normal');
hold on
[p, S] = polyfit(bend, twist, 1);
[y, delta] = polyval(p, x, S);
plot(x, y, 'r');
plot(x, y + 2 * delta, 'r--', x, y - 2 * delta, 'r--');
xlabel('Bend (Degrees)')
ylabel('Twist AoA (Degrees)')
hold off

% animated dot
%figure(4)
%xlabel('Bend (Degrees)')
%ylabel('Twist AoA (Degrees)')
%for i=1:10:length(t)
%    plot(bend(i),twist(i),'or','MarkerSize',5,'MarkerFaceColor','r')
%    axis([-40 30 -30 30])
%    pause(.05)
%end

