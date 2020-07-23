function AnimateObject(obj, orientation)
    t = [0:0.005:1]';
    r = [0*t, 0*t, 0*t];
    A = [orientation(1)*t, orientation(2)*t, orientation(3)*t];
    h = patch('Faces', obj.shape.faces, 'Vertices', obj.shape.vertices);
    set(h, 'FaceColor', [0.8 0.8 1.0], 'EdgeColor', 'none',...
           'FaceLighting', 'gouraud', 'AmbientStrength', 0.15);
    camlight('headlight');
    material('dull');
    axis('image');
    view([-135 35]);
    set(gca,'FontSize',14);
    axis vis3d equal;
    grid on;
    xlabel('x','FontSize',14);
    ylabel('y','FontSize',14);
    zlabel('z','FontSize',14);
    xlim([-2.5,2.5]);
    ylim([-2.5,2.5]);
    zlim([-2.5,2.5]);

    % the object is rendering with the centre of mass not at the origin of the
    % coordinate system. This means that when the rotation are applied the
    % satellite rotates not about its center of mass. Shift it over a hardcoded
    % 1.5 (15 cm in the simulation). This should be corrected and generalized
    obj.shape.vertices(:, 2) = obj.shape.vertices(:, 2) - 1.5;

    while 1
        for i=1:length(t)
            rotatedObj = obj.reorient(A(i, :));
            set(h, 'Faces', rotatedObj.shape.faces);
            set(h, 'Vertices', rotatedObj.shape.vertices);
            drawnow;
        end
    end
end
