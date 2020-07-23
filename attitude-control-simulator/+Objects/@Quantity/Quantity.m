classdef Quantity
    properties
        value; unit; dimension;
    end
    methods
        function obj = Quantity(value, unit)
        obj.value = value; obj.unit = unit; obj.dimension = length(value);
        end
    end
end
