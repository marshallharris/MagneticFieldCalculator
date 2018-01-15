function Btot = MagneticFluxMagnitude(conductorVector, measurementPosition)
nConductors = size(conductorVector,2);
Bxin = zeros(nConductors, 1);
Byin = zeros(nConductors, 1);
Bxout = zeros(nConductors, 1);
Byout = zeros(nConductors, 1);

for n = 1:nConductors
    Bxin(n) = MagneticFluxXInPhase(conductorVector(n), measurementPosition);
    Byin(n) = MagneticFluxYInPhase(conductorVector(n), measurementPosition);
    Bxout(n) = MagneticFluxXOutPhase(conductorVector(n), measurementPosition);
    Byout(n) = MagneticFluxYOutPhase(conductorVector(n), measurementPosition);
end

Btot = sqrt(sum(Bxin)^2 + sum(Byin)^2 + sum(Bxout)^2 + sum(Byout)^2);
end