function [filteredBlock, voice] = filter_block(inputBlock, voice)
% ---------------------------------------------------------------
% FILTER_BLOCK - Aplica el filtro definido en voice.filter usando banco RC
% ---------------------------------------------------------------

filtType = voice.filter.type;
cutoff   = voice.filter.cutoff;
fs       = voice.filter.fs;

if ~isfield(voice.filter, 'bank') || isempty(voice.filter.bank)
    switch filtType
        case 'lowpass'
            voice.filter.bank = lowpass([], fs, []);
        otherwise
            filteredBlock = inputBlock;
            return;
    end
end

[~, idx] = min(abs([voice.filter.bank.cutoff] - cutoff));

b = voice.filter.bank(idx).b;
a = voice.filter.bank(idx).a;
state = voice.filter.bank(idx).state;

fBlock = zeros(size(inputBlock));
for n = 1:length(inputBlock)
    fBlock(n) = b*inputBlock(n) + a*state;
    state = fBlock(n);
end

voice.filter.bank(idx).state = state;
filteredBlock = fBlock;

end
