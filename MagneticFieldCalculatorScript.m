%simple vertical conductor
clear
close all

spacingBetweenWires = 4.267;
lineCurrent1 = 500;

conductor1a = Conductor(lineCurrent1, 2*pi/3, [-6.096, 12.0]);
conductor1b = Conductor(lineCurrent1, 0, [-5.715, 19.85]);
conductor1c = Conductor(lineCurrent1, -2*pi/3, [-5.486, 27.62]);

conductor2a = Conductor(lineCurrent1, 2*pi/3, [6.096, 12.0]);
conductor2b = Conductor(lineCurrent1, 0, [5.715, 19.85]);
conductor2c = Conductor(lineCurrent1, -2*pi/3, [5.486, 27.62]);

conductorVector = [conductor1a, conductor1b, conductor1c, conductor2a, conductor2b, conductor2c];

xPosition = linspace(-20.0, 20.0, 4000);

Btot= zeros(1,4000); 

for n = 1:4000
    measurementPosition = [xPosition(n), 4.0];
    Btot(n) = MagneticFluxMagnitude(conductorVector, measurementPosition);
end

plot(xPosition, Btot)
maxValue = max(Btot)