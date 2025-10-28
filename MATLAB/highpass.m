function bank = highpass(~, fs, cutoffList)
% ---------------------------------------------------------------
% HIGHPASS - Precompute 1st-order highpass filters (RC) más preciso
% ---------------------------------------------------------------
if nargin < 3 || isempty(cutoffList)
    cutoffList = logspace(log10(20), log10(fs/2*0.95), 50);
end

bank = struct('cutoff', {}, 'alpha', {}, 'state', {}, 'xPrev', {});

for k = 1:length(cutoffList)
    fc = cutoffList(k);
    RC = 1/(2*pi*fc);
    dt = 1/fs;
    alpha = RC / (RC + dt);  % coef highpass clásico

    bank(k).cutoff = fc;
    bank(k).alpha  = alpha;
    bank(k).state  = 0;      % y[n-1]
    bank(k).xPrev  = 0;      % x[n-1] para la diferencia
end
end
