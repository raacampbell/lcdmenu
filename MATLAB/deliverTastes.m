function params=deliverTastes(params,verbose)
% function params=deliverTastes(params,verbose)
%
% Purpose
% Present tastes using the linear actuator and a filter paper strip. Parameters for
% stimulus delivery are specified by a structure, params. If this
% is not provided, default values are used. Not all fields in
% params need to be defined since defaults are applied.
%
% Inputs
% params - optional structure with stimulus parameter information.
% verbose - display detailed information on hardware states to screen.
%           Default is 0.
%
% Outputs
% params - data structure defining what was presented when. This is
% also saved to disk.
%
%
%
% Rob Campbell - Jan 2014


% Handle input arguments
error(nargchk(1,2,nargin));
if nargin<2, verbose=0; end

%Make default stimulus parameters if needed
if nargin==0
    params=defaultTasteparams;
elseif nargin>0
    params=defaultTasteparams(params);
end %Set up default parameters

saveFileStr=['params_',datestr(now,'yymmdd_HHMMss')];


%Create the vector of odours to present
if length(params(1).stimLatency)==1
    params=setUpTasteStimuli(params);
end


%Estimate and display the time taken for stimulus presentation.
timeTaken=0;
for P=1:length(params)
    timeTaken=timeTaken+sum(params(P).isi);
end

fprintf('\nStimulus delivery will take %s, finishing at about %s.\n',...
    prettyTime(timeTaken,0), datestr(rem(now+(timeTaken/(24*60^2)),1)))




%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%Stimulus delivery loop
global otherData 
otherData=struct(...
    'timestamp',[],...
    'data',[]);



for P=1:length(params)
    if length(params)>1
        fprintf('**** presenting block %d/%d ****\n', P, length(params))
    end
    
    presentParams(params(P))
end

%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%




%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
    function presentParams(theseParams)

        if length(otherData)==1 && isempty(otherData.data)
            N=1;
        else
            N=length(otherData)+1;
        
        end

        for ii=1:length(theseParams.duration)            
            otherData(N).timestamp(ii)=now;  %Store the times the stimuli were delivered
            
            %Display message to screen            
            ID=theseParams.stimulus(ii);
            fprintf('%d/%d, presenting taste at position %d\n',...
                    ii,length(theseParams.duration), ID);
            
            
            
            %-------------------------------
            %Execute whatever is in the pre-stim function if needed
            if ~isempty(theseParams.preStimFunc)
                theseParams.preStimFunc();
            end            
            %-------------------------------
                
            %-------------------------------
            triggerScan
            T1=now;
            presentThisTaste(theseParams,ii,verbose);
            T2=now;
            ISI=theseParams.isi(ii)- (T2-T1)*24*60^2;
            if verbose
                fprintf('Waiting %0.2fs (ISI)',ISI)
            end
            pause(ISI)
            %-------------------------------
            

         
            save(saveFileStr,'params')

        end        

    end





end

