clear; close all; clc;
import Objects.*
import Util.*
import Visualizers.*
position = [0; 0; 0];
orientation = [0; 0; 0];
stlPath = './test.stl';
qsat = Objects.CubeSat('3U', position, orientation, stlPath);
Visualizers.AnimateReposition(qsat, [4*pi, 0*pi, 2*pi]);
