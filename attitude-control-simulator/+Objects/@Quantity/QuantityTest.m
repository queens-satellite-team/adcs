classdef QuantityTest < matlab.unittest.TestCase
	methods(Test)

		function testQuantityOrderAndAccuracy(testCase)
			actval = Quantity(10,'m');
			ecptout = (10,'m',1);
			testCase.verifyEqual(actval,ecptout)
		end
	end
end
