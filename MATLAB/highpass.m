function bank = highpass(~, fs, cutoffList)
% ---------------------------------------------------------------
% HIGHPASS - Precompute simple 1st-order highpass filters (RC)
% ---------------------------------------------------------------
% Inputs:
%   fs         - sampling frequency
%   cutoffList - vector de frecuencias de corte (Hz), opcional
%
% Output:
%   bank - struct array con campos:
%       cutoff : frecuencia de corte
%       b      : coeficiente para x[n] - x[n-1]
%       a      : coeficiente para y[n-1]
%       state  : estado previo

if nargin < 3 || isempty(cutoffList)
    cutoffList = logspace(log10(20), log10(fs/2*0.95), 50); % 50 filtros log-scale
end

bank = struct('cutoff', {}, 'b', {}, 'a', {}, 'state', {});

for k = 1:length(cutoffList)
    fc = cutoffList(k);
    RC = 1/(2*pi*fc);
    dt = 1/fs;
    alpha = RC/(RC + dt);  % coeficiente RC discreto para highpass
    bank(k).cutoff = fc;
    bank(k).b = alpha;
    bank(k).a = alpha;
    bank(k).state = 0; % inicial
end
end
