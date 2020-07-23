classdef Shape
    properties
        faces; vertices; type;
    end
    methods
        function self = Shape(vargin)
            if nargin > 1
                self.faces = faces; self.vertices = vertices;
            elseif nargin == 1
                shape = vargin; % i.e. "cube", "pyramid" etc.
                self.type = shape;
                [self.vertices, self.faces] = Util.CreateVerticesAndFaces(shape);
            end
        end

        function self = convertStl(self, stlPath)
            polygonStruct = Util.Stl2Patch(stlPath);
            self.faces = polygonStruct.faces;
            self.vertices = polygonStruct.vertices;
        end
    end
end

%TO TEST method (Shape) with 2 possible input types
%give vargin input 'cube' and check numrows(self.faces) = 6 and
%numrow(self.vertices) = 8
%give vargin input '6, 8' (or does the input have to be a vector?) and carry
%out the same check

	%does self. pass the classes' properties to one of it's methods?

%TO TEST method (convertStl)
%LEARN WHAT THE HELL IS GOING ON IN STL FUNCTION
