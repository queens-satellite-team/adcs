function AnimateObject(obj, position)
    t = [0:0.005:1]';       
    r = [0*t, 0*t, 0*t];   
    P = [position(1)*t, position(2)*t, position(3)*t];
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

    obj.shape.vertices(:, 2) = obj.shape.vertices(:, 2) - 1.5;
    while 1
        for i=1:length(t)
            newObj = obj.reposition(P(i, :));
            set(h, 'Faces', newObj.shape.faces);
            set(h, 'Vertices', newObj.shape.vertices);
            drawnow;
        end
    end
end
