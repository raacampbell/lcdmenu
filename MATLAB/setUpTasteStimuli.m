function params=setUpTasteStimuli(params)
% function params=setUpTasteStimuli(params)
%
% Convert the parameters structure (defaultTasteparams) into a format
% for presenting to the fly. Everything will be presented from a
% BrainWare-like (www.tdt.com/software/for_neuro.htm) "stimulus
% grid", so we specify all stim parameters for each trial. This
% makes it easy to present whatever stimulus we want on a trial by
% trial basis.
%
%
% Rob Campbell - Jan 2014
%


%stimulus parameters (more can be added if they are numeric fields
%in params)
stimGridFields={'duration','stimLatency','stimulus','isi'};


for pp=1:length(params)
    params(pp)=workerFunction(params(pp));
end

    function theseParams=workerFunction(theseParams)
        
        % To allow for mixtures (cell array in the .odour field)
        change =0;
        if iscell(theseParams.stimulus)
            originalstimulus = theseParams.stimulus;
            theseParams.stimulus = 1:length(theseParams.stimulus);
            change = 1;
        end
        
        %Generate one repeat of the "stimulus-grid"
        for ii=1:length(stimGridFields)
            sGrid{ii}=theseParams.(stimGridFields{ii});
        end
        sGrid=setprod(sGrid{:});
        
        
        %replicate grid
        sGrid=repmat(sGrid,[1,1,theseParams.reps]);
        
        %block-randomize
        if theseParams.randomize && size(sGrid,1)>1
            % added this part so that the same stimulus will not be repeated when
            % blocks are changed
            zrep = 1;
            while zrep
                for ii=1:size(sGrid,3)
                    sGrid(:,:,ii)=sGrid(randperm(size(sGrid,1)),:,ii);
                end
              tmp=prod(sGrid,2);
              zrep=~all(diff(tmp(:)));
            end
        end
        
        
        %return to 2-D
        sGrid=reshape(permute(sGrid,[1,3,2]),...
            size(sGrid,1)*size(sGrid,3),size(sGrid,2));
        
        
        %replace vectors
        for i=1:length(stimGridFields)
            theseParams.(stimGridFields{i})=sGrid(:,i);
        end
        
        % put cell array back if mixtures were used
        if change
            for j=1:length(sGrid(:,3)) % 3 being the stimulus field
                odrs(j) = originalstimulus(sGrid(j,3));
            end
            theseParams.stimulus = odrs;
        end
            
    end


end

