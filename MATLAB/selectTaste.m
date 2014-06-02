function selectTaste(taste)

% Move servo to desired taste. There are four possible tastes.
% taste will take on a number between 1 and 4


taste=dec2bin(taste-1,2);
putvalue(dio.Line(2),str2num(taste(1)) )
putvalue(dio.Line(3),str2num(taste(2)) )
