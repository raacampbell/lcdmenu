function presentThisTaste(params,ii,verbose)
%
% present taste stimulus defined in params stim matrix index ii
% this is a helper function for deliverTastes
% It works with the Tastulator 3000


if nargin<3
    verbose=0;
end

t=params.stimulus(ii);


tic

if verbose
    fprintf('waiting %ds... ',params.stimLatency(ii))
end

%Make sure actuator is retracted and select taste
actuator('retract',verbose); %just to be sure
selectTaste(t,verbose); %select taste

%Wait the pre-stimulus time
pause(params.stimLatency(ii)-toc)


%Deliver stimulus for required time
if verbose
    fprintf('moving up to fly... ')
end

actuator('extend',verbose);
pause(params.duration(ii))

%Retract taste
tic

if verbose
    fprintf('returning... \n')
end
actuator('retract',verbose)
