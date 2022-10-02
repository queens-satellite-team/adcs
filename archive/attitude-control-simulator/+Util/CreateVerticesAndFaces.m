function [vertices, faces] = CreateVerticesAndFaces(shape)
    switch shape
        case 'cube'
            vertices = [0, 0, 0;
                        1, 0, 0;
                        0, 1, 0;
                        0, 0, 1;
                        1, 1, 0;
                        0, 1, 1;
                        1, 0, 1;
                        1, 1, 1];
            faces = [1, 2, 5, 3;
                     1, 3, 6, 4;
                     1, 4, 7, 2;
                     4, 7, 8, 6;
                     2, 5, 8, 7;
                     3, 6, 8, 5];

    end
end

%how does this account for multiple cubes being lined up?

%LOW GRAIN
%just check numcol(vertices) = 3 and numcol(faces) = 4

%FINE GRAIN
%check number of rows in faces and vertices for each shape
%i.e. for cube numrows(vertices) = 8 and numrows(faces) = 6
