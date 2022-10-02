function tests = CubeSatTest
tests = functiontests(localfunctions);
end

function testUnit(testCase)
verifyNotEqual(units,NULL);
end

	%FINE GRAIN
%function testDimensions(testCase)
%verifyNotEqual(Dimensions,NULL);
%end

function testWeight(testCase)
verifyNotEqual(weight,NULL);
end


