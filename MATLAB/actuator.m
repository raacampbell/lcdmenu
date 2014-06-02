% function turnes taste delivery into position of respective odor delivery


function actuator(ReEx)
%Move linear actuator to extended or retracted position
%
% ReEx can be the strings 'extend' or 'retract'

switch lower(ReEx)
case 'extend'
	putvalue(dio.Line(3),1);
case 'retract'
	putvalue(dio.Line(3),0);
otherwise
	error('Unkown option')
end



