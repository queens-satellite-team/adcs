function h = GetPatch(object)
    for i=1:length(object.shape.faces)
        PatchData_X(:, i) = object.shape.vertices(object.shape.faces(i, :), 1);
        PatchData_Y(:, i) = object.shape.vertices(object.shape.faces(i, :), 2);
        PatchData_Z(:, i) = object.shape.vertices(object.shape.faces(i, :), 3);
    end
    h = patch(PatchData_X, PatchData_Y, PatchData_Z, 'Y');
end
