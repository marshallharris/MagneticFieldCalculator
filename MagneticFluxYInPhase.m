function Byin = MagneticFluxYInPhase(I, phase, X, Y)
Iphase = I*cos(phase);
permeability = 4*pi*10^-7;

Byin = (permeability * X * Iphase)/(2*pi*(X^2 + Y^2));
end