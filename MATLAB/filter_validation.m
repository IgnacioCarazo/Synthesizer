%% ---------------------------------------------------------------
% Test Synth engine: Filtered vs Unfiltered
% ---------------------------------------------------------------
fprintf('[%0.3f s] Entering <engine>\n', toc);
fs = 44100;
waveType = 'saw';
blockSize = 128;
tic;

% -------------------------------
% MIDI message ~= (MIDI notes, velocity, duration)
% -------------------------------
melody1 = [
    60 0.8 1;   % C4 ~ 261 Hz
    64 0.8 1;   % E4 ~ 329 Hz
    67 0.8 1;   % G4 ~ 392 Hz
    72 0.8 1    % C5 ~ 523 Hz
    ];

% -------------------------------
% Initialize VoiceManager template
% -------------------------------
defaultVoice = struct( ...
    'active', false, ...
    'note', [], ...
    'velocity', 0, ...
    'osc', [], ...
    'phase', 0, ...
    'samplesRemaining', 0, ...
    'adsr', struct('attack',0.1,'decay',0.1,'sustain',0.7,'release',0.3), ...
    'env', struct('stage','idle','stageSampleCounter',0,'level',0,'releaseStartLevel',0), ...
    'filter', struct('type','highpass','fs',fs,'cutoff',500,'resonance',0.7,'state',[]) ...
    );

vm.numVoices = 1;
vm.voices = repmat(defaultVoice, 1, vm.numVoices);

% -------------------------------
% Create unfiltered copy
% -------------------------------
vmNoFilter = vm;
vmNoFilter.voices(1).filter.cutoff = 20000;  % effectively no filtering

% -------------------------------
% Preallocate output
% -------------------------------
totalSamples = sum(melody1(:,3))*fs + vm.numVoices*fs*0.5;
finalWaveFiltered   = zeros(1, ceil(totalSamples));
finalWaveUnfiltered = zeros(1, ceil(totalSamples));

writePos = 1;
noteIndex1 = 1;

% -------------------------------
% Main synthesis loop
% -------------------------------
while noteIndex1 <= size(melody1,1) || any([vm.voices.active])
    % --- Assign new note to filtered VM ---
    if ~vm.voices(1).active && noteIndex1 <= size(melody1,1)
        vm.voices(1).active = true;
        vm.voices(1).note = melody1(noteIndex1,1:2);
        vm.voices(1).samplesRemaining = round(melody1(noteIndex1,3)*fs);
        vm.voices(1).env.stage = 'attack';
        vm.voices(1).env.stageSampleCounter = 0;
        vm.voices(1).env.level = 0;
        vm.voices(1).env.releaseStartLevel = 0;
        noteIndex1 = noteIndex1 + 1;
    end

    % --- Assign same note to unfiltered VM ---
    if ~vmNoFilter.voices(1).active
        vmNoFilter.voices(1).active = true;
        vmNoFilter.voices(1).note = melody1(1,1:2);
        vmNoFilter.voices(1).samplesRemaining = round(melody1(1,3)*fs);
        vmNoFilter.voices(1).env.stage = 'attack';
        vmNoFilter.voices(1).env.stageSampleCounter = 0;
        vmNoFilter.voices(1).env.level = 1;  % full amplitude
        vmNoFilter.voices(1).env.releaseStartLevel = 1;
    end

    % --- Generate audio blocks ---
    [blockFiltered, vm]       = voicemanager(vm, waveType, fs, blockSize);
    [blockUnfiltered, vmNoFilter] = voicemanager(vmNoFilter, waveType, fs, blockSize);

    % --- Write into arrays ---
    finalWaveFiltered(writePos:writePos+length(blockFiltered)-1)   = blockFiltered;
    finalWaveUnfiltered(writePos:writePos+length(blockUnfiltered)-1) = blockUnfiltered;
    writePos = writePos + length(blockFiltered);
end

finalWaveFiltered   = finalWaveFiltered(1:writePos-1);
finalWaveUnfiltered = finalWaveUnfiltered(1:writePos-1);

% -------------------------------
% Frequency-domain analysis (FFT) with stem
% -------------------------------
N = length(finalWaveFiltered);
Y_filtered   = fft(finalWaveFiltered);
Y_unfiltered = fft(finalWaveUnfiltered);

f = (0:N-1)*(fs/N);
half = 1:floor(N/2);

figure;
stem(f(half), 20*log10(abs(Y_unfiltered(half))), 'r', 'DisplayName','Unfiltered'); hold on;
stem(f(half), 20*log10(abs(Y_filtered(half))), 'b', 'DisplayName','Filtered');
xlabel('Frequency (Hz)');
ylabel('Magnitude');
title('Frequency Spectrum: Filtered vs Unfiltered');
legend; grid on;
xlim([0 10000]);  % focus on harmonic range

% -------------------------------
% Optional: Play filtered waveform
% -------------------------------
% sound(finalWaveFiltered, fs);
