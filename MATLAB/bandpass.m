function bank = bandpass(~, fs, cutoffList)
% ---------------------------------------------------------------
% BANDPASS - Precompute simple 1st-order bandpass filters (HP+LP)
% ---------------------------------------------------------------
% Inputs:
%   fs         - sampling frequency
%   cutoffList - Nx2 matrix: [f_low, f_high] para cada filtro
%
% Output:
%   bank - struct array con campos:
%       cutoff : [f_low, f_high]
%       b_hp, a_hp : coeficientes highpass
%       b_lp, a_lp : coeficientes lowpass
%       state_hp, state_lp : estados previos

if nargin < 3 || isempty(cutoffList)
    cutoffList = [logspace(log10(20), log10(fs/4), 25)' ...
        logspace(log10(fs/4), log10(fs/2*0.95), 25)'];
end

bank = struct('cutoff', {}, 'b_hp', {}, 'a_hp', {}, 'b_lp', {}, 'a_lp', {}, 'state_hp', {}, 'state_lp', {});

for k = 1:size(cutoffList,1)
    f_low  = cutoffList(k,1);
    f_high = cutoffList(k,2);

    % Highpass coef RC
    RC_hp = 1/(2*pi*f_low);
    dt = 1/fs;
    alpha_hp = RC_hp/(RC_hp + dt);

    % Lowpass coef RC
    RC_lp = 1/(2*pi*f_high);
    alpha_lp = dt/(RC_lp + dt);

    bank(k).cutoff = [f_low, f_high];

    % Highpass
    bank(k).b_hp = alpha_hp;
    bank(k).a_hp = alpha_hp;
    bank(k).state_hp = 0;

    % Lowpass
    bank(k).b_lp = alpha_lp;
    bank(k).a_lp = 1 - alpha_lp;
    bank(k).state_lp = 0;
end
end
