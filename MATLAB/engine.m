%% ---------------------------------------------------------------
% Test Synth engine (with per-voice ADSR)
% ---------------------------------------------------------------
fprintf('[%0.3f s] Entering <engine>\n', toc);
fs = 44100;           % Sampling rate (Hz)
waveType = 'square';     % Waveform type: 'sine','triangle','square','saw'
blockSize = 128;      % Samples per audio block
tic;
% -------------------------------
% MIDI message ~= (MIDI notes, velocity, duration)
% -------------------------------

melody1 = [
    44 0.8 3;
    ];

melody2 = [
    ];

% -------------------------------
% Initialize VoiceManager with per-voice ADSR defaults using repmat
% -------------------------------
defaultVoice = struct( ...
    'active', false, ...
    'note', [], ...
    'velocity', 0, ...
    'osc', [], ...
    'phase', 0, ...
    'samplesRemaining', 0, ...
    'adsr', struct( ...
    'attack', 2.7, ...
    'decay', 0.1, ...
    'sustain', 0.1, ...
    'release', 0.1 ...
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
    'cutoff', 2000, ...
    'resonance', 0.7, ...
    'state', [] ...
    ) ...
    );

vm.numVoices = 1;
vm.voices = repmat(defaultVoice, 1, vm.numVoices);

% Index trackers for each melody
noteIndex1 = 1; noteIndex2 = 1;

% -------------------------------
% Estimate total samples to preallocate (rough)
% -------------------------------
totalSamples = sum(melody1(:,3));
totalSamples = totalSamples * fs + vm.numVoices * fs * 0.5; % small cushion
finalWave = zeros(1, ceil(totalSamples));  % preallocate

writePos = 1;  % track where to write in finalWave

% -------------------------------
% Main synthesis loop
% -------------------------------
while noteIndex1 <= size(melody1,1) || noteIndex2 <= size(melody2,1) || any([vm.voices.active])

    % Assign new notes if voices are free
    if ~vm.voices(1).active && noteIndex1 <= size(melody1,1)
        vm.voices(1).active = true;
        vm.voices(1).note = melody1(noteIndex1,1:2);
        vm.voices(1).samplesRemaining = round(melody1(noteIndex1,3)*fs);
        % initialize envelope for this new note
        vm.voices(1).env.stage = 'attack';
        vm.voices(1).env.stageSampleCounter = 0;
        vm.voices(1).env.level = 0;
        vm.voices(1).env.releaseStartLevel = 0;
        fprintf('Voice 1 STARTED note %d\n', melody1(noteIndex1,1));
        noteIndex1 = noteIndex1 + 1;
    end

    % if ~vm.voices(2).active && noteIndex2 <= size(melody2,1)
    %     vm.voices(2).active = true;
    %     vm.voices(2).note = melody2(noteIndex2,1:2);
    %     vm.voices(2).samplesRemaining = round(melody2(noteIndex2,3)*fs);
    %     vm.voices(2).env.stage = 'attack';
    %     vm.voices(2).env.stageSampleCounter = 0;
    %     vm.voices(2).env.level = 0;
    %     vm.voices(2).env.releaseStartLevel = 0;
    %     fprintf('Voice 2 STARTED note %d\n', melody2(noteIndex2,1));
    %     noteIndex2 = noteIndex2 + 1;
    % end

    % Generate mixed audio block from all voices (voicemanager returns updated vm)
    [block, vm] = voicemanager(vm, waveType, fs, blockSize);

    % -------------------------------
    % Write block into preallocated finalWave
    % -------------------------------
    finalWave(writePos:writePos+length(block)-1) = block;
    writePos = writePos + length(block);
end
fprintf('[%0.3f s] Exiting <engine>\n', toc);
% Trim any extra zeros
finalWave = finalWave(1:writePos-1);

% -------------------------------
% Play the final waveform
% -------------------------------
sound(finalWave, fs);

% -------------------------------
% Plot the final waveform
% -------------------------------
t = (0:length(finalWave)-1)/fs;
figure;
plot(t, finalWave, 'b');
xlabel('Time [s]');
ylabel('Amplitude');
grid on;
title('Final Synth Waveform');
