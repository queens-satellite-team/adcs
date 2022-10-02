function R = Euler2R(A)
    a1 = A(1);
    a2 = A(2);
    a3 = A(3);

    R1 = [1, 0, 0;
        0, cos(a1), -sin(a1);
        0, sin(a1), cos(a1)];

    R2 = [cos(a2), 0, sin(a2);
        0, 1, 0;
        -sin(a2), 0, cos(a2)];

    R3 = [cos(a3), -sin(a3), 0;
        sin(a3), cos(a3), 0;
        0, 0, 1];

    R = R1*R2*R3;

%LOW GRAIN
%check that the output is a 3x3 vector

%HIGH GRAIN
%check that all values present in the 3x3 vector are less than 1
