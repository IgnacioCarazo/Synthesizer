function [waveforms, noteCycle, phaseIncrement] = square(noteFreq, fs, velocity)
% ---------------------------------------------------------------
% Generate a single-cycle square wave using precalculated wavetable banks
% ---------------------------------------------------------------
%
% Inputs/Outputs: same as sine
%
% Notes:
%   - Only odd harmonics are included
%   - Amplitude of each harmonic ~ 1/n
% ---------------------------------------------------------------

banks = [20, 25, 30, 35, 40, 45, 50, 56, 63, 70, 79, 88, 99, 111, 125, ...
    140, 157, 176, 198, 222, 249, 279, 313, 352, 395, 443, 497, 558, ...
    627, 704, 790, 886, 992, 1111, 1244, 1393, 1560, 1747, 1955, 2187, ...
    2450, 2740, 3070, 3440, 3860, 4330, 4860, 5450, 6120, 6870, 7720, ...
    8680, 9760, 10960, 12320, 13840, 15530, 17440, 19550, 20000];

fNyquist = fs / 2;
desired_number_of_harmonics = 128;
waveforms = cell(1, length(banks));

for b = 1:length(banks)
    bankFreq = banks(b);
    Ns = round(fs / bankFreq);
    n = 0:Ns-1;

    % Determine harmonics
    if bankFreq > 10000
        desired_harmonics_for_bank = 32;
    else
        desired_harmonics_for_bank = desired_number_of_harmonics;
    end
    maxHarmonics = floor(0.9 * fNyquist / bankFreq);
    number_of_harmonics = min(desired_harmonics_for_bank, maxHarmonics);

    k = 1:2:(2*number_of_harmonics-1); % odd harmonics
    a = 1 ./ k;

    waveform = zeros(1, Ns);
    for hi = 1:length(k)
        waveform = waveform + a(hi) * sin(2*pi*k(hi)*n/Ns);
    end

    waveforms{b} = waveform / max(abs(waveform));
end

% Interpolation
noteFreq = max(min(noteFreq, banks(end)), banks(1));
[~, bankIndex] = min(abs(banks - noteFreq));
bankWave = waveforms{bankIndex};

Ns_new = round(fs / noteFreq);
t_old = linspace(0,1,length(bankWave));
t_new = linspace(0,1,Ns_new);
noteCycle = interp1(t_old, bankWave, t_new, 'linear');
noteCycle = velocity * (noteCycle / max(abs(noteCycle)));
phaseIncrement = noteFreq / fs;

end
