
%Josh made this edit in vi because somehow he doesn't have matlab... what!!! 
%simple vertical conductor
clear
close all
heightToCentralWire = 24.079;
spacingBetweenWires = 4.267;
distanceFromCentralWire  = [0, -23.079];
lineCurrent = 350;

% in phase components
%left most conductor
Bxin_a = MagneticFluxXInPhase(lineCurrent, -2*pi/3, distanceFromCentralWire(1) + spacingBetweenWires, distanceFromCentralWire(2));
Byin_a = MagneticFluxYInPhase(lineCurrent, -2*pi/3, distanceFromCentralWire(1) + spacingBetweenWires, distanceFromCentralWire(2));

%central
Bxin_b = MagneticFluxXInPhase(lineCurrent, 0, distanceFromCentralWire(1), distanceFromCentralWire(2));
Byin_b = MagneticFluxYInPhase(lineCurrent, 0, distanceFromCentralWire(1), distanceFromCentralWire(2));

%right 
Bxin_c = MagneticFluxXInPhase(lineCurrent, 2*pi/3, distanceFromCentralWire(1) - spacingBetweenWires, distanceFromCentralWire(2));
Byin_c = MagneticFluxYInPhase(lineCurrent, 2*pi/3, distanceFromCentralWire(1) - spacingBetweenWires, distanceFromCentralWire(2));


% out of phase components
Bxout_a = MagneticFluxXOutPhase(lineCurrent, -2*pi/3, distanceFromCentralWire(1) + spacingBetweenWires, distanceFromCentralWire(2));
Byout_a = MagneticFluxYOutPhase(lineCurrent, -2*pi/3, distanceFromCentralWire(1) + spacingBetweenWires, distanceFromCentralWire(2));

%central
Bxout_b = MagneticFluxXOutPhase(lineCurrent, 0, distanceFromCentralWire(1), distanceFromCentralWire(2));
Byout_b = MagneticFluxYOutPhase(lineCurrent, 0, distanceFromCentralWire(1), distanceFromCentralWire(2));

%right 
Bxout_c = MagneticFluxXOutPhase(lineCurrent, 2*pi/3, distanceFromCentralWire(1) - spacingBetweenWires, distanceFromCentralWire(2));
Byout_c = MagneticFluxXOutPhase(lineCurrent, 2*pi/3, distanceFromCentralWire(1) - spacingBetweenWires, distanceFromCentralWire(2));


Bxin = Bxin_a + Bxin_b + Bxin_c;
Byin = Byin_a + Byin_b + Byin_c;
Bxout = Bxout_a + Bxout_b + Bxout_c;
Byout = Byout_a + Byout_b + Byout_c;

Btot = sqrt(Bxin^2 + Byin^2 + Bxout^2 + Byout^2);
