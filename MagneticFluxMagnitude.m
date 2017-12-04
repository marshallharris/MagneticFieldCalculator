function Btot = MagneticFluxMagnitude(spacingBetweenWires, positionRelativeToCentralWire, lineCurrent)

% in phase components
%left most conductor
Bxin_a = MagneticFluxXInPhase(lineCurrent, -2*pi/3, positionRelativeToCentralWire(1) + spacingBetweenWires, positionRelativeToCentralWire(2));
Byin_a = MagneticFluxYInPhase(lineCurrent, -2*pi/3, positionRelativeToCentralWire(1) + spacingBetweenWires, positionRelativeToCentralWire(2));

%central
Bxin_b = MagneticFluxXInPhase(lineCurrent, 0, positionRelativeToCentralWire(1), positionRelativeToCentralWire(2));
Byin_b = MagneticFluxYInPhase(lineCurrent, 0, positionRelativeToCentralWire(1), positionRelativeToCentralWire(2));

%right 
Bxin_c = MagneticFluxXInPhase(lineCurrent, 2*pi/3, positionRelativeToCentralWire(1) - spacingBetweenWires, positionRelativeToCentralWire(2));
Byin_c = MagneticFluxYInPhase(lineCurrent, 2*pi/3, positionRelativeToCentralWire(1) - spacingBetweenWires, positionRelativeToCentralWire(2));

% out of phase components
Bxout_a = MagneticFluxXOutPhase(lineCurrent, -2*pi/3, positionRelativeToCentralWire(1) + spacingBetweenWires, positionRelativeToCentralWire(2));
Byout_a = MagneticFluxYOutPhase(lineCurrent, -2*pi/3, positionRelativeToCentralWire(1) + spacingBetweenWires, positionRelativeToCentralWire(2));

%central
Bxout_b = MagneticFluxXOutPhase(lineCurrent, 0, positionRelativeToCentralWire(1), positionRelativeToCentralWire(2));
Byout_b = MagneticFluxYOutPhase(lineCurrent, 0, positionRelativeToCentralWire(1), positionRelativeToCentralWire(2));

%right 
Bxout_c = MagneticFluxXOutPhase(lineCurrent, 2*pi/3, positionRelativeToCentralWire(1) - spacingBetweenWires, positionRelativeToCentralWire(2));
Byout_c = MagneticFluxYOutPhase(lineCurrent, 2*pi/3, positionRelativeToCentralWire(1) - spacingBetweenWires, positionRelativeToCentralWire(2));


Bxin = Bxin_a + Bxin_b + Bxin_c;
Byin = Byin_a + Byin_b + Byin_c;
Bxout = Bxout_a + Bxout_b + Bxout_c;
Byout = Byout_a + Byout_b + Byout_c;

Btot = sqrt(Bxin^2 + Byin^2 + Bxout^2 + Byout^2);
end