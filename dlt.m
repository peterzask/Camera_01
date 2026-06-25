
%Camera_outFile
%A=DLT_(1:12,:);
%A = DLT_'*DLT_;
%[u s v]=svd(A);
%p=v(:,12) %/ v(12,12);
%P=[p(1:4)';p(5:8)';p(9:12)']

Camera_outFile

P=Perspective

A=P(1:3,1:3)
b=P(:,4)
a1=A(1,:)';
a2=A(2,:)';
a3=A(3,:)';
%check for valid Perspective Projection Matrix, See pg 45 in Forsyth and Ponce ,"Modern Computer Vision"
if(0!=det(A)) disp('det(A) != 0, success'); else disp('det(A) = 0, failure');endif
if(0==cross(a1,a3)'*cross(a2,a3)) disp('cross(a1,a3)''*cross(a2,a3) = 0, success');
                             else disp('cross(a1,a3)''*cross(a2,a3) != 0, failure');endif
if(cross(a1,a3)'*cross(a1,a3) == cross(a2,a3)'*cross(a2,a3))
    disp('cross(a1,a3)''*cross(a1,a3) == cross(a2,a3)''*cross(a2,a3), success'); else
    disp('cross(a1,a3)''*cross(a1,a3) != cross(a2,a3)''*cross(a2,a3)), failure');endif



rho=1/norm(a3)
r3 = rho*a3
u0=rho^2*(a1'*a3)
v0=rho^2*(a2'*a3)
theta = acos(cross(a1,a3)'*cross(a2,a3))/(norm(cross(a1,a3))*norm(cross(a2,a3)))
alpha = rho^2*norm(cross(a1,a3))*sin(theta)
beta  = rho^2*norm(cross(a2,a3))*sin(theta)
r1=cross(a2,a3)/norm(cross(a2,a3))
r2=cross(r3,r1)
Ka=[alpha 0 u0;0 beta v0;0 0 1]
t = rho*inv(K)*b
disp('The sign on tz, eg t(3), tells with image is in front of camera or behind');
disp('That can be changed by changing epsilon above,')

