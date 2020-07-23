restart;
with(DynamicSystems):
with(LinearAlgebra):
with(VectorCalculus):
Is:=Matrix(3,3,symbol=Isi); # inertia matrix of satellite
invtIs:=MatrixInverse(Is);
w:=<w1(t),w2(t),w3(t)>; # angular velocity of satellite
g:=<q1(t),q2(t),q3(t)>; # orientation of satellite
h:=<h1(t),h2(t),h3(t)>; # angular momentum of reaction wheels
Nc:=<Nc1(t),Nc2(t),Nc3(t)>; # control torque
Ne:=<Ne1(t),Ne2(t),Ne3(t)>; # external distrubance torques

x:=<w,g,h>; # state vector
u:=Nc; # input vector
sys:=<diff(w,t)=-invtIs.CrossProduct(w,Is.w)-invtIs.CrossProduct(w,Is.h)+invtIs.Nc+invtIs.Ne,
            diff(q,t)=1/2*IdentityMatrix(3).w, 
            diff(h,t)=-Nc>; # non-linear state equation
sys:=convert(convert(sys,Vector),list);
DynamicSystems:-EquilibriumPoints(sys,[u]
