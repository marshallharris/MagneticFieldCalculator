%simple vertical conductor
clear
close all
heightToCentralWire = 24.079;
spacingBetweenWires = 4.267;

permeability = 4*pi*10^-7;

lineCurrent = 350;
centralWireRealCurrent = cos(0)*lineCurrent; 
centralWireImaginaryCurrent = sin(0)*lineCurrent;
topWireRealCurrent = cos(2*pi/3) * lineCurrent;
topWireImaginaryCurrent = sin(2*pi/3) * lineCurrent;
bottomWireRealCurrent = cos(-2*pi/3) * lineCurrent;
bottomWireImaginaryCurrent = sin(-2*pi/3) * lineCurrent;

vertDistanceFromCentralConductor = -23.079;
horzDistanceFromCentralConductor = 0;

B0xIn = permeability * vertDistanceFromCentralConductor * centralWireRealCurrent / (2*pi*(vertDistanceFromCentralConductor^2 + horzDistanceFromCentralConductor^2));
B0yin = permeability * horzDistanceFromCentralConductor * centralWireRealCurrent / (2*pi*(vertDistanceFromCentralConductor^2 + horzDistanceFromCentralConductor^2));

B0xOut = permeability * vertDistanceFromCentralConductor * centralWireImaginaryCurrent / (2*pi*(vertDistanceFromCentralConductor^2 + horzDistanceFromCentralConductor^2));
B0yOut = permeability * horzDistanceFromCentralConductor * centralWireImaginaryCurrent / (2*pi*(vertDistanceFromCentralConductor^2 + horzDistanceFromCentralConductor^2));

