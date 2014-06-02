% function turnes taste deliverz into position of respective odor delivery


function selectTasteLeg(taste)

if taste==0
    t=[0,0];
end

if taste==1
    t=[1,0];
end

if taste==2
    t=[0,1];
end

if taste==3
    t=[1,1];
end

dio=digitalio('nidaq','Dev1');
addline(dio,1:2,'out');
putvalue(dio.Line(1:2),t)
vall=getvalue(dio);

% s=daq.createSession('ni');
% s.addDigitalChannel('Dev1','Port1/Line1:2','bidirectional');
% s.Channels(1).Direction='Output';
% s.Channels(2).Direction='Output';
% s.outputSingleScan([t])


% s.Channels(1).Direction='Input';
% s.Channels(2).Direction='Input';
% data=s.inputSingleScan;
% data