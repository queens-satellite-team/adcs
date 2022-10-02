classdef CubeSat < Objects.Object3D 
    properties
        units;
    end
    methods
        function self = CubeSat(varargin)
            if nargin == 3
                [units, position, orientation] = varargin{:};
            elseif nargin == 4
                [units, position, orientation, stl] = varargin{:};
            elseif nargin < 3
                error('Too few arguments');
            elseif nargin > 4
                error('Too many arguments');
            end
            if units == '3U'
                units = 3;
            elseif units == '2U'
                units = 2;
            elseif units == '1U'
                units = 1;
            else
                error('Supported units are 3U, 2U and 1U');
            end
            dimensions = [0.1; 0.1; units*0.1];
            weight = units*1;
            try
                shape = Objects.Shape();
                shape = shape.convertStl(stl);
            catch
                shape = Objects.Shape('cube');
                Lx = dimensions(1); Ly = dimensions(2); Lz = dimensions(3);
                shape.vertices = [Lx*ones(8, 1), Ly*ones(8, 1), Lz*ones(8, 1)]...
                                .* shape.vertices;
            end

            self = self@Objects.Object3D(position, orientation, dimensions, weight,...
                                                                shape);
            self.units = units;
        end
        function self = reorient(self, newOrientation)
            self.orientation = Objects.Quantity(newOrientation, 'rad');
            alpha = self.orientation.value(1);
            beta = self.orientation.value(2);
            gamma = self.orientation.value(3);

            Rx = [1, 0, 0;
                  0, cos(alpha), -sin(alpha);
                  0, sin(alpha), cos(alpha)];
            Ry = [cos(beta), 0, sin(beta);
                  0, 1, 0;
                  -sin(beta), 0, cos(beta)];
            Rz = [cos(gamma), -sin(gamma), 0;
                  sin(gamma), cos(gamma), 0;
                  0, 0, 1];
            R = Rx * Ry * Rz;

            for i=1:length(self.shape.vertices)
                self.shape.vertices(i, :) = self.position.value' +...
                                              self.shape.vertices(i, :) * R';
            end
        end
    end
end
