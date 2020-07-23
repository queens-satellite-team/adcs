classdef EulerToQuaternion
	properties
		q0; q1; q2; q3;
    end
    methods
		function self = EulerToQuaternion(eulAngles)
            if length(eulAngles) == 3
                %eulAngles come in as (z,y,x) array
                z = eulAngles(1);
                y = eulAngles(2);
                z = eulAngles(3);
                cy = cos(z*0.5);
                sy = sin(z*0.5);
                cp = cos(y*0.5);
                sp = sin(y*0.5);
                cr = cos(z*0.5);
                sr = sin(z*0.5);
                self.q0 = cy*cp*cr+sy*sp*sr;
                self.q1 = cy*cp*sr-sy*sp*cr;
                self.q2 = sy*cp*sr+cy*sp*cr;
                self.q3 = sy*cp*cr-cy*sp*sr;
            end
        end
        function vPrime = rotationMatrix(self,v)
            %input is 3 row array
            rotationMatrix = [(1-2*self.q0^2-2*self.q3^2),2*(self.q1*self.q2+self.q0*self.q3),2*(self.q1*self.q3-self.q0*self.q2);
                         2*(self.self.q1*self.q2-self.q0*self.q3),(1-2*self.q1^2-2*self.q3^2),2*(self.q2*self.q3-self.q0*self.q1);
                         2*(self.q1*self.q3+self.q0*self.q2),2*(self.q2*self.q3-self.q0*self.q1),(1-2*self.q1^2-2*self.q2^2)];
            vPrime = rotationMatrix*v;
        end
    end
end
