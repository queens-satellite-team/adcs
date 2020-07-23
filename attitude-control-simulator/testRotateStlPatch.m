clear; close all; clc;
import Objects.*
import Util.*
import Visualizers.*
position = [0; 0; 0];
orientation = [0; 0; 0];
stlPath = './test.stl';
qsat = Objects.CubeSat('3U', position, orientation);
orientation = [0; 0; 0];
qsat = qsat.reorient([-pi/3, 0, pi/3]);
h = patch('Faces', qsat.shape.faces, 'Vertices', qsat.shape.vertices);
set(h, 'FaceColor', [0.8 0.8 1.0], 'EdgeColor', 'none',...
       'FaceLighting', 'gouraud', 'AmbientStrength', 0.15);
camlight('headlight');
material('dull');
axis('image');
view([-135 35]);

%can a test be written that will run the code without actual animations?
