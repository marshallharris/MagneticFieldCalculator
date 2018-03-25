%simple vertical conductor
clear
close all

lineCurrent1 = 231;

conductor1a = Conductor(lineCurrent1, 2*pi/3, [-3.96, 20.5]);
conductor1b = Conductor(lineCurrent1, 0, [-5.48, 26.67]);
conductor1c = Conductor(lineCurrent1, -2*pi/3, [-3.96, 32.76]);

conductor2a = Conductor(lineCurrent1, -2*pi/3, [3.96, 20.5]);
conductor2b = Conductor(lineCurrent1, 0, [5.48, 26.67]);
conductor2c = Conductor(lineCurrent1, 2*pi/3, [3.96, 32.76]);

conductorVector = [conductor1a, conductor1b, conductor1c, conductor2a, conductor2b, conductor2c];

xPosition = linspace(-20.0, 20.0, 100);
yPosition = linspace(0.0, 45.0, 100);

% Btot= zeros(1, 10000); 

PositionMagneticFieldMatrix = zeros(100,100);

for n = 1:100
    for i = 1:100
        measurementPosition = [xPosition(i), yPosition(n)];
        Btot = MagneticFluxMagnitude(conductorVector, measurementPosition);
        PositionMagneticFieldMatrix(n, i) = Btot;
    end
end

C = gradient(PositionMagneticFieldMatrix);
mesh(xPosition, yPosition, PositionMagneticFieldMatrix)
title('3D visualization of magnetic field strength surrounding 240kV type L tower')
xlabel('horizontal distance (m)');
ylabel('vertical distance (m)');
zlabel('magnetic flux (T)');
%contourf(PositionMagneticFieldMatrix)
zlim([0.000001 0.00009]);

figure
v= [0.00001 0.00009 0.00008 0.00007 0.00006 0.00005 0.00004 0.00003 0.00002 0.00001 0.000005]
[C,h] = contourf(xPosition, yPosition, PositionMagneticFieldMatrix, v)
title('Contour of magnetic field strength surrounding 240kV type L tower')
xlabel('horizontal distance (m)');
ylabel('vertical distance (m)');
clabel(C, h, v);
maxValue = max(Btot)

% calculate magnetic field in a vertical line directly up the middle of the
% line
xPosition = 0;
yPosition = linspace(0.0, 20.5, 42);
verticalMagneticFieldVector = zeros(42,1);
for n = 1:42
    measurementPosition = [xPosition, yPosition(n)];
    Btot = MagneticFluxMagnitude(conductorVector, measurementPosition);
    verticalMagneticFieldVector(n) = Btot;
end

figure
plot(20.5 - yPosition, verticalMagneticFieldVector);
title('Magnetic Field Strength Along Vertically Under 240Kv Type L tower');
xlabel('Vertical Distance (m)');
ylabel('Magnetic Flux Density (T)');