function actuator(ReEx,verbose)
%Move linear actuator to extended or retracted position
%
% ReEx can be the strings 'extend' or 'retract'
%
% The tastulator takes care of which position  to actually move to. 
%
% Rob Campbell June 2014

if nargin<2
  verbose=0;
end

%Only connect 6509 if needed. 
global USB6009;
if isempty(USB6009)
	if verbose
	    fprintf('Connecting USB-6509\n')
	end

	USB6009=connect6009;
end



switch lower(ReEx)
case 'extend'
    if getvalue(USB6009.do.Line(9))==1
        return
    end
    
	putvalue(USB6009.do.Line(9),1);
    if verbose, fprintf('Extending\n'), end
case 'retract'
    if getvalue(USB6009.do.Line(9))==0
        return
    end
    
	putvalue(USB6009.do.Line(9),0);
    if verbose, fprintf('Retracting\n'), end

otherwise
	error('Unkown option')
end



