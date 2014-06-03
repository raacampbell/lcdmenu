function H=connect6009
% connect6009 for stuff such as triggering shock and tastulator control
%
%

hw=daqhwinfo('nidaq');
id=strmatch('USB-6009',hw.BoardNames);


ai=eval(hw.ObjectConstructorName{id,1});
addchannel(ai,0:1);
set(ai,'SampleRate',1E3)

ao=eval(hw.ObjectConstructorName{id,2});
addchannel(ao,0);


%Electric shock
do=eval(hw.ObjectConstructorName{id,3});
addline(do,0:11,'out');


H.ai=ai;
H.ao=ao;
H.do=do;