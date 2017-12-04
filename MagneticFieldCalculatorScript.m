%simple vertical conductor
clear
close all

spacingBetweenWires = 4.267;
lineCurrent = 350;

xPosition = linspace(-200.0, 200.0, 4000);


Btot= zeros(1,4000);

for n = 1:4000
    positionRelativeToCentralWire= [xPosition(n), -23.079];
    Btot(n) = MagneticFluxMagnitude(spacingBetweenWires, positionRelativeToCentralWire, lineCurrent);
end

plot(xPosition, Btot)
