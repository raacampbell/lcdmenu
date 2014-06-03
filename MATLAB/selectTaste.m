function selectTaste(taste,verbose)
% Move servo to desired taste. There are four possible tastes.
%
% "taste" can take on a valuebetween 1 and 4
%
% Rob Campbell - June 2014


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


taste=dec2bin(taste-1,2);
if verbose
    fprintf('Binary output: %s\n', taste)
end

putvalue(USB6009.do.Line(10),str2num(taste(1)) )
putvalue(USB6009.do.Line(11),str2num(taste(2)) )
