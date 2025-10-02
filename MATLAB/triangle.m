function [waveforms, noteCycle, phaseIncrement] = triangle(noteFreq, fs, velocity)
% ---------------------------------------------------------------
% Generate a single-cycle triangle wave using precalculated wavetable banks
% ---------------------------------------------------------------
%
% This function creates a single-cycle triangle waveform for a given note frequency
% using precomputed wavetable banks. Only odd harmonics are included with amplitudes
% scaled according to the triangle wave formula. The waveform is phase-aware and
% ready for continuous playback.
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
%   - Only odd harmonics are used to synthesize the triangle wave, with alternating signs.
%   - The note frequency is constrained to the available bank range.
% ---------------------------------------------------------------

% Central frequencies for the banks
banks = [20, 40, 80, 160, 320, 640, 1280, 5120];

% Nyquist frequency
fNyquist = fs / 2;

% Maximum number of desired harmonics per bank
desired_number_of_harmonics = 64;

% Initialize cell array to store normalized waveforms
waveforms = cell(1, length(banks));

% Generate wavetable banks
for b = 1:length(banks)
    bankFreq = banks(b);

    % Number of samples per period
    Ns = round(fs / bankFreq);
    n = 0:Ns-1;

    % Maximum harmonics allowed by Nyquist
    maxHarmonics = floor(fNyquist / bankFreq);
    number_of_harmonics = min(desired_number_of_harmonics, maxHarmonics);

    % Harmonic vector and amplitudes (triangle wave formula, odd harmonics)
    k = 1:number_of_harmonics;
    a = mod(k,2) ./ (k.^2);       % odd harmonics only
    a = a .* ((-1).^( (k-1)/2 )); % alternate sign for triangle wave

    % Initialize waveform
    waveform = zeros(1, Ns);

    % Sum harmonics
    for harmonic_index = 1:number_of_harmonics
        waveform = waveform + a(harmonic_index) * sin(2*pi*harmonic_index*n/Ns);
    end

    % Normalize waveform
    waveform_normalized = waveform / max(abs(waveform));

    % Store in cell array
    waveforms{b} = waveform_normalized;
end

% -------------------
% Select closest bank
% -------------------
% Limit noteFreq to bank range to avoid out-of-range errors
noteFreq = max(min(noteFreq, banks(end)), banks(1));
[~, bankIndex] = min(abs(banks - noteFreq));
bankWave = waveforms{bankIndex};

% Interpolation to match exact note frequency (single cycle)
Ns_new = round(fs / noteFreq);
t_old = linspace(0, 1, length(bankWave));
t_new = linspace(0, 1, Ns_new);
noteCycle = interp1(t_old, bankWave, t_new, 'linear');

% Normalize and apply velocity
noteCycle = velocity * (noteCycle / max(abs(noteCycle)));

% Compute phase increment for continuous playback
phaseIncrement = noteFreq / fs;

end
