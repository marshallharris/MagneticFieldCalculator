function Bxout = MagneticFluxXOutPhase(conductor, measurementPoint)
Iphase = conductor.current*sin(conductor.phase);
permeability = 4*pi*10^-7;

X = measurementPoint(1) - conductor.position(1);
Y = measurementPoint(2) - conductor.position(2);

Bxout = -(permeability * Y * Iphase)/(2*pi*(X^2 + Y^2));
end