clear; close all; clc;
import Util.*
patchData = Util.Stl2Patch('test.stl');
patch(patchData, 'FaceColor', [0.8 0.8 1.0], 'EdgeColor', 'none',...
                 'FaceLighting', 'gouraud',...
                 'AmbientStrength', 0.15);
camlight('headlight');
material('dull');
axis('image');
view([-135 35]);
