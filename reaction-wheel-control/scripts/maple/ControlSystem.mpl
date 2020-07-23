restart;
with(LinearAlgebra):
with(VectorCalculus);
Is:=Matrix(3,3,symbol=Isi); # inertia matrix of satellite
invtIs:=MatrixInverse(Is);
w:=<w1,w2,w3>; # angular velocity of satellite
q:=<q1,q2,q3,q4>; # orientation of satellite
h:=<h1,h2,h3>; # angular momentum of reaction wheels
Nc:=<Nc1,Nc2,Nc3>;
Ne:=<Ne1,Ne2,Ne3>;

x:=<w,q,h>; # state vector
u:=-Nc;
f:=convert(<-invtIs.CrossProduct(w,Is.w)-invtIs.CrossProduct(w,Is.h)+invtIs.Nc+invtIs.Ne,
1/2*Matrix([[0,w[3],-w[2],w[1]],
            [-w[3],0,w[1],w[2]],
            [w[2],-w[1],0,w[3]],
            [-w[1],-w[2],-w[3],0]]).q, 
-Nc>, Vector); # non-linear state equation
subs([q1=1/2*w1,q2=1/2*w2,q3=1/2*w3,q4=1],f);
A:=simplify(Jacobian(f,convert(x,list)));
