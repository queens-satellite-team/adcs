function tests = Euler2RTest
tests = functiontests(localfunctions);
end

function generalTest(testCase)
	A = [0,0,0];
	actVals = Util.Euler2R(A);
	ecptVals = [1,0,0;0,1,0;0,0,1];
	verifyEqual(testCase,actVals,ecptVals)
end

function negativeTest(testCase)
	A = [-pi/2,pi/2,pi/2];
	actVals = Util.Euler2R(A);
	ecptVals = [0,0,1;0,1,0;-1,0,0];
    verifyEqual(testCase,actVals,ecptVals)
end

%function inputArrayCheck(testCase)