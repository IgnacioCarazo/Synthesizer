function [filteredBlock, voice] = filter_block(inputBlock, voice)
% ---------------------------------------------------------------
% FILTER_BLOCK - Aplica el filtro definido en voice.filter usando bancos RC
% ---------------------------------------------------------------

filtType = voice.filter.type;
cutoff   = voice.filter.cutoff;
fs       = voice.filter.fs;

% Inicializar banco si no existe
if ~isfield(voice.filter, 'bank') || isempty(voice.filter.bank)
    switch filtType
        case 'lowpass'
            voice.filter.bank = lowpass([], fs, []);
        case 'highpass'
            voice.filter.bank = highpass([], fs, []);
        case 'bandpass'
            voice.filter.bank = bandpass([], fs, []);
        otherwise
            filteredBlock = inputBlock;
            return;
    end
end

fBlock = zeros(size(inputBlock));

switch filtType
    case 'lowpass'
        [~, idx] = min(abs([voice.filter.bank.cutoff] - cutoff));
        b = voice.filter.bank(idx).b;
        a = voice.filter.bank(idx).a;
        state = voice.filter.bank(idx).state;

        for n = 1:length(inputBlock)
            fBlock(n) = b*inputBlock(n) + a*state;
            state = fBlock(n);
        end

        voice.filter.bank(idx).state = state;

    case 'highpass'
        [~, idx] = min(abs([voice.filter.bank.cutoff] - cutoff));
        alpha = voice.filter.bank(idx).alpha;
        state = voice.filter.bank(idx).state;
        xPrev = voice.filter.bank(idx).xPrev;

        for n = 1:length(inputBlock)
            fBlock(n) = alpha * (state + inputBlock(n) - xPrev);
            state = fBlock(n);
            xPrev = inputBlock(n);
        end

        voice.filter.bank(idx).state = state;
        voice.filter.bank(idx).xPrev = xPrev;

    case 'bandpass'
        % Seleccionar filtro más cercano
        dist = sum(abs(cat(1, voice.filter.bank.cutoff) - cutoff), 2);
        [~, idx] = min(dist);

        % Extraer coef y estados
        b_hp = voice.filter.bank(idx).b_hp; a_hp = voice.filter.bank(idx).a_hp; s_hp = voice.filter.bank(idx).state_hp; xPrev_hp = 0;
        b_lp = voice.filter.bank(idx).b_lp; a_lp = voice.filter.bank(idx).a_lp; s_lp = voice.filter.bank(idx).state_lp;

        for n = 1:length(inputBlock)
            y_hp = b_hp * (inputBlock(n) - xPrev_hp) + a_hp * s_hp;  % highpass
            s_hp = y_hp;
            xPrev_hp = inputBlock(n);

            y_lp = b_lp * y_hp + a_lp * s_lp;                        % lowpass
            s_lp = y_lp;

            fBlock(n) = y_lp;
        end

        voice.filter.bank(idx).state_hp = s_hp;
        voice.filter.bank(idx).state_lp = s_lp;
end

filteredBlock = fBlock;

end
