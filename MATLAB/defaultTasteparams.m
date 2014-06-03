function params=defaultTasteparams(params)
%  function params=defaultTasteparams
%
% Create a parameter structure with the default values specified by
% the body of this function. Any of these parameters can be
% modified or others added providing a partial parameters structure
% as an argument to this function. 
%
% e.g.
% params.reps=10;  
% params=defaultTasteparams(params)
%
% The params structure is then fed to deliverTastes, to define what will 
% be presented to the fly. Note that within that function we convert the
% structure to stimulus matrix using this:
% params=setUpTasteStimuli(params);
%
% Rob Campbll - Jan 2014

  
%Set up default parameters
if nargin==0
  params=struct;
end


defaults={...
    'duration',6;...
    'stimLatency',8;...
    'stimulus',[1,2,3,4];... 
    'reps',4;...
    'isi',35;...
    'randomize',0;...
    'preStimFunc',''};


for P=1:length(params)
    tmpP(P)=addDefaults(params(P));
end
params=tmpP;

%----------------------------------------------------
    function tmp=addDefaults(tmp)
        for i=1:size(defaults,1)
            if ~isfield(tmp,defaults{i,1})
                tmp.(defaults{i,1})=defaults{i,2};
            end
        end                      
    end

end
