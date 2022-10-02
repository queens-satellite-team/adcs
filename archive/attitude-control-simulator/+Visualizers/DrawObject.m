function [PatchData_X, PatchData_Y, PatchData_Z] = DrawObject(object)
    alpha = object.orientation.value(1);
    beta = object.orientation.value(2);
    gamma = object.orientation.value(3);

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

    for i=1:length(object.shape.vertices)
        VertexData(i, :) = object.position.value' + object.shape.vertices(i, :) * R';
    end

    for i=1:length(object.shape.faces)
        PatchData_X(:, i) = VertexData(object.shape.faces(i, :), 1);
        PatchData_Y(:, i) = VertexData(object.shape.faces(i, :), 2);
        PatchData_Z(:, i) = VertexData(object.shape.faces(i, :), 3);
    end
end
