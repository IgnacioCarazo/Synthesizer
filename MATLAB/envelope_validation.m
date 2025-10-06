%% ---------------------------------------------------------------
% Test Synth engine (with per-voice ADSR) + envelope comparison
% ---------------------------------------------------------------
fprintf('[%0.3f s] Entering <engine>\n', toc);
fs = 44100;           % Sampling rate (Hz)
waveType = 'saw';     % Waveform type: 'sine','triangle','square','saw'
blockSize = 128;      % Samples per audio block
tic;

% -------------------------------
% MIDI message ~= (MIDI notes, velocity, duration)
% -------------------------------
melody1 = [64 0.8 3];  % single note
melody2 = [];

% -------------------------------
% Initialize VoiceManager with per-voice ADSR defaults
% -------------------------------
defaultVoice = struct( ...
    'active', false, ...
    'note', [], ...
    'velocity', 0, ...
    'osc', [], ...
    'phase', 0, ...
    'samplesRemaining', 0, ...
    'adsr', struct( ...
    'attack', 0.1, ...
    'decay', 0.1, ...
    'sustain', 0.7, ...
    'release', 0.3 ...
    ), ...
    'env', struct( ...
    'stage', 'idle', ...
    'stageSampleCounter', 0, ...
    'level', 0, ...
    'releaseStartLevel', 0 ...
    ), ...
    'filter', struct( ...
    'type', 'lowpass', ...
    'fs', fs, ...
    'cutoff', 300, ...
    'resonance', 0.7, ...
    'state', [] ...
    ) ...
    );

vm.numVoices = 1;
vm.voices = repmat(defaultVoice, 1, vm.numVoices);

% -------------------------------
% Create a copy for no-envelope comparison
% -------------------------------
vmNoEnv = vm;
vmNoEnv.voices(1).adsr.attack  = 0;
vmNoEnv.voices(1).adsr.decay   = 0;
vmNoEnv.voices(1).adsr.sustain = 1;  % full amplitude
vmNoEnv.voices(1).adsr.release = 0;

% -------------------------------
% Estimate total samples to preallocate
% -------------------------------
totalSamples = sum(melody1(:,3));
totalSamples = totalSamples * fs + vm.numVoices * fs * 0.5; % small cushion
finalWaveEnv   = zeros(1, ceil(totalSamples));  % with envelope
finalWaveNoEnv = zeros(1, ceil(totalSamples));  % no envelope

writePos = 1;

% -------------------------------
% Index tracker
% -------------------------------
noteIndex1 = 1;

% -------------------------------
% Main synthesis loop
% -------------------------------
while noteIndex1 <= size(melody1,1) || any([vm.voices.active])

    % --- Assign new note to normal vm ---
    if ~vm.voices(1).active && noteIndex1 <= size(melody1,1)
        vm.voices(1).active = true;
        vm.voices(1).note = melody1(noteIndex1,1:2);
        vm.voices(1).samplesRemaining = round(melody1(noteIndex1,3)*fs);
        vm.voices(1).env.stage = 'attack';
        vm.voices(1).env.stageSampleCounter = 0;
        vm.voices(1).env.level = 0;
        vm.voices(1).env.releaseStartLevel = 0;
        fprintf('Voice 1 STARTED note %d\n', melody1(noteIndex1,1));
        noteIndex1 = noteIndex1 + 1;
    end

    % --- Assign same note to no-envelope vm ---
    if ~vmNoEnv.voices(1).active
        vmNoEnv.voices(1).active = true;
        vmNoEnv.voices(1).note = melody1(1,1:2);
        vmNoEnv.voices(1).samplesRemaining = round(melody1(1,3)*fs);
        vmNoEnv.voices(1).env.stage = 'attack';
        vmNoEnv.voices(1).env.stageSampleCounter = 0;
        vmNoEnv.voices(1).env.level = 1;         % full amplitude
        vmNoEnv.voices(1).env.releaseStartLevel = 1;
    end

    % --- Generate audio blocks ---
    [blockEnv, vm]       = voicemanager(vm, waveType, fs, blockSize);
    [blockNoEnv, vmNoEnv] = voicemanager(vmNoEnv, waveType, fs, blockSize);

    % --- Write into preallocated arrays ---
    finalWaveEnv(writePos:writePos+length(blockEnv)-1)   = blockEnv;
    finalWaveNoEnv(writePos:writePos+length(blockNoEnv)-1) = blockNoEnv;
    writePos = writePos + length(blockEnv);
end

% Trim extra zeros
finalWaveEnv   = finalWaveEnv(1:writePos-1);
finalWaveNoEnv = finalWaveNoEnv(1:writePos-1);

fprintf('[%0.3f s] Exiting <engine>\n', toc);

% -------------------------------
% Plot waveform comparison
% -------------------------------
% t = (0:length(finalWaveEnv)-1)/fs;
% figure;
% plot(t, finalWaveNoEnv, 'r', 'DisplayName','No Envelope'); hold on;
% plot(t, finalWaveEnv, 'b', 'DisplayName','With Envelope');
% xlabel('Time [s]'); ylabel('Amplitude');
% legend; grid on;
% title('Envelope Comparison');

% -------------------------------
% Frequency-domain analysis (FFT)
% -------------------------------
% N = length(finalWaveNoEnv);
% Y_noEnv = fft(finalWaveNoEnv);
% Y_env   = fft(finalWaveEnv);

% f = (0:N-1)*(fs/N); % frequency axis

% % Only plot positive frequencies
% half = 1:floor(N/2);

% figure;
% % plot(f(half), abs(Y_noEnv(half)), 'r', 'DisplayName','No Envelope'); hold on;
% plot(f(half), abs(Y_env(half)), 'b', 'DisplayName','With Envelope');
% xlabel('Frequency (Hz)');
% ylabel('Magnitude');
% title('Frequency Spectrum');
% legend; grid on;
% xlim([0 5000]); % zoom to relevant range

% -------------------------------
% Frequency-domain analysis (FFT) with stem
% -------------------------------
N = length(finalWaveNoEnv);
Y_noEnv = fft(finalWaveNoEnv);
Y_env   = fft(finalWaveEnv);

f = (0:N-1)*(fs/N); % frequency axis
half = 1:floor(N/2); % only positive frequencies

figure;
stem(f(half), abs(Y_noEnv(half)), 'r', 'DisplayName','No Envelope'); hold on;
stem(f(half), abs(Y_env(half)), 'b', 'DisplayName','With Envelope');
xlabel('Frequency (Hz)');
ylabel('Magnitude');
title('Frequency Spectrum (Discrete Harmonics)');
legend; grid on;
xlim([0 2000]); % zoom to relevant harmonic range


% -------------------------------
% Play waveform with envelope
% -------------------------------
% sound(finalWaveEnv, fs);
