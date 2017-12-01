function Byout = MagneticFluxYOutPhase(I, phase, X, Y)
Iphase = I*sin(phase);
permeability = 4*pi*10^-7;

Byout = (permeability * X * Iphase)/(2*pi*(X^2 + Y^2));
end