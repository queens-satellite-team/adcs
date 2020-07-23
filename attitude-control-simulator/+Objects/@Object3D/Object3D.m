classdef Object3D
    properties
        position; orientation; dimensions; weight; shape;
    end
    methods
        function self = Object3D(position, orientation, dimensions, weight, shape)
            if isrow(position) | isrow(orientation) | isrow(dimensions)
                error('Position, orientation and dimensions must be row vectors');
            end
            if ~isscalar(weight)
                error('Weight must be a scalar');
            end
            [self.position, self.orientation, self.dimensions] =...
                deal(Objects.Quantity(double.empty(3, 0), 'm'));
            self.weight = Objects.Quantity(double.empty, 'kg');
            self.shape = Objects.Shape.empty;
            if (nargin > 0)
                temp = {position, orientation, dimensions, weight};
                [self.position.value, self.orientation.value,...
                self.dimensions.value, self.weight.value] = temp{:};
                self.shape = shape;
            end
        end
        %{
        function updatePose(position, orientation)
            self.position = position;
            rotationMatrix3D = Eul2R(angle);
            f or i=1:length(self.shape.vertices)
                self.shape.vertices(i,:) = 
        %}
    end
end
