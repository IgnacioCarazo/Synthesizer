function [waveforms, noteCycle, phaseIncrement] = saw(noteFreq, fs, velocity)
% ---------------------------------------------------------------
% Generate a single-cycle sawtooth wave using precalculated wavetable banks
% ---------------------------------------------------------------
%
% This function creates a single-cycle sawtooth waveform for a given note frequency
% using precomputed wavetable banks. Harmonics are truncated per bank
% to reduce aliasing. The waveform is phase-aware and ready for continuous playback.
%
% Inputs:
%   noteFreq        - Frequency of the note to play (Hz)
%   fs              - Sampling rate (Hz)
%   velocity        - Amplitude scaling (0 to 1)
%
% Outputs:
%   waveforms       - Cell array of all precalculated wavetable banks
%   noteCycle       - Single-cycle waveform for the given note frequency
%   phaseIncrement  - Amount to increment phase per sample (for continuous playback)
%
% Notes:
%   - The function limits harmonics per bank to avoid aliasing.
%   - The note frequency is constrained to the available bank range.
% ---------------------------------------------------------------

% Central frequencies of the banks (logarithmic spacing)
banks = [20, 25, 30, 35, 40, 45, 50, 56, 63, 70, 79, 88, 99, 111, 125, ...
    140, 157, 176, 198, 222, 249, 279, 313, 352, 395, 443, 497, 558, ...
    627, 704, 790, 886, 992, 1111, 1244, 1393, 1560, 1747, 1955, 2187, ...
    2450, 2740, 3070, 3440, 3860, 4330, 4860, 5450, 6120, 6870, 7720, ...
    8680, 9760, 10960, 12320, 13840, 15530, 17440, 19550, 20000];

% Nyquist frequency
fNyquist = fs / 2;

% Maximum number of desired harmonics per bank
desired_number_of_harmonics = 128;

% Initialize cell array to store normalized waveforms
waveforms = cell(1, length(banks));

% Generate wavetable banks
for b = 1:length(banks)
    bankFreq = banks(b);

    Ns = round(fs / bankFreq); % Number of samples per period
    n = 0:Ns-1;

    % -------------------
    % Compute number of harmonics safely
    % -------------------
    if bankFreq > 10000
        desired_harmonics_for_bank = 32;  % fewer harmonics for high banks
    else
        desired_harmonics_for_bank = desired_number_of_harmonics;
    end
    maxHarmonics = floor(0.9 * fNyquist / bankFreq);  % safety margin
    number_of_harmonics = min(desired_harmonics_for_bank, maxHarmonics);

    k = 1:number_of_harmonics;
    a = 1 ./ k;

    waveform = zeros(1, Ns);

    for harmonic_index = 1:number_of_harmonics
        waveform = waveform + a(harmonic_index) * sin(2*pi*harmonic_index*n/Ns);
    end

    waveform_normalized = waveform / max(abs(waveform));
    waveforms{b} = waveform_normalized;
end

% -------------------
% Select closest bank
% -------------------
% Limit noteFreq to bank range to avoid out-of-range errors
noteFreq = max(min(noteFreq, banks(end)), banks(1));
[~, bankIndex] = min(abs(banks - noteFreq));
bankWave = waveforms{bankIndex};

% Interpolation to match the note frequency (single cycle)
Ns_new = round(fs / noteFreq);
t_old = linspace(0, 1, length(bankWave));
t_new = linspace(0, 1, Ns_new);
noteCycle = interp1(t_old, bankWave, t_new, 'linear');

% Normalize and apply velocity
noteCycle = velocity * (noteCycle / max(abs(noteCycle)));

% Compute phase increment for continuous playback
phaseIncrement = noteFreq / fs;

figure;
plot(noteCycle);
title('Single-Cycle Triangle Waveform');
xlabel('Sample');
ylabel('Amplitude');
grid on;

end
