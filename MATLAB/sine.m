function [waveforms, noteCycle, phaseIncrement] = sine(noteFreq, fs, velocity)
% ---------------------------------------------------------------
% Generate a single-cycle sine wave using precalculated wavetable banks
% ---------------------------------------------------------------
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
%   - Only the fundamental harmonic is needed for a sine wave.
% ---------------------------------------------------------------

banks = [20, 25, 30, 35, 40, 45, 50, 56, 63, 70, 79, 88, 99, 111, 125, ...
    140, 157, 176, 198, 222, 249, 279, 313, 352, 395, 443, 497, 558, ...
    627, 704, 790, 886, 992, 1111, 1244, 1393, 1560, 1747, 1955, 2187, ...
    2450, 2740, 3070, 3440, 3860, 4330, 4860, 5450, 6120, 6870, 7720, ...
    8680, 9760, 10960, 12320, 13840, 15530, 17440, 19550, 20000];

fNyquist = fs / 2;
waveforms = cell(1, length(banks));

% Generate wavetable banks (fundamental only)
for b = 1:length(banks)
    bankFreq = banks(b);
    Ns = round(fs / bankFreq);
    n = 0:Ns-1;
    waveform = sin(2*pi*n/Ns);  % fundamental only
    waveforms{b} = waveform / max(abs(waveform)); % normalize
end

% -------------------
% Select closest bank and interpolate
% -------------------
noteFreq = max(min(noteFreq, banks(end)), banks(1));
[~, bankIndex] = min(abs(banks - noteFreq));
bankWave = waveforms{bankIndex};

Ns_new = round(fs / noteFreq);
t_old = linspace(0,1,length(bankWave));
t_new = linspace(0,1,Ns_new);
noteCycle = interp1(t_old, bankWave, t_new, 'linear');

% Normalize and apply velocity
noteCycle = velocity * (noteCycle / max(abs(noteCycle)));

% Compute phase increment
phaseIncrement = noteFreq / fs;

end
