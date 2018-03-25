classdef Conductor
    %CONDUCTOR This class contains properties of a conductor such as it's
    %position, current and phase

    properties
        current
        phase
        position %x and y
    end
    
    methods
        function obj = Conductor(current, phase, position)
            obj.current = current;
            obj.phase = phase;
            obj.position = position;
        end
    end
end

