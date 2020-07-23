restart;
with(DynamicSystems):
with(LinearAlgebra):
with(VectorCalculus):
Is:=Matrix(3,3,symbol=Isi); # inertia matrix of satellite
invtIs:=MatrixInverse(Is);
w:=<w1,w2,w3>; # angular velocity of satellite
g:=<q1,q2,q3>; # orientation of satellite
h:=<h1,h2,h3>; # angular momentum of reaction wheels
Nc:=<Nc1,Nc2,Nc3>; # control torque
Ne:=<Ne1,Ne2,Ne3>; # external distrubance torques

x:=<w,g,h>; # state vector
u:=Nc; # input vector
f:=convert(<-invtIs.CrossProduct(w,Is.w)-invtIs.CrossProduct(w,Is.h)+invtIs.Nc+invtIs.Ne,
            1/2*IdentityMatrix(3).w, 
            -Nc>, Vector); # non-linear state equation
A:=Jacobian(f,convert(x,list));
Bu:=Jacobian(f,convert(u,list));
Bd:=Jacobian(f,convert(Ne,list));
Cx:=Matrix([IdentityMatrix(6),Matrix(6,3)]);
Du:=Matrix(6,3);
Equal(<Bu[1],Bu[2],Bu[3]>,invtIs,compare=all); # test that upper 3 x 3 is inverted inertia matrix
Equal(<Bd[1],Bd[2],Bd[3]>,invtIs,compare=all); # test that upper 3 x 3 is inverted inertia matrix
