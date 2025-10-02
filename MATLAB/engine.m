%% ---------------------------------------------------------------
% Test Synth engine
% ---------------------------------------------------------------

fs = 44100;           % Sampling rate (Hz)
waveType = 'saw';     % Waveform type: 'sine','triangle','square','saw'
blockSize = 128;      % Samples per audio block

% -------------------------------
% MIDI message ~= (MIDI notes, velocity, duration)
% -------------------------------

melody1 = [
    64 0.8 0.5; 65 0.8 0.5; 67 0.8 0.5; 69 0.8 0.5;
    71 0.8 1.0; 69 0.8 0.5; 67 0.8 0.5; 65 0.8 1.0
    ];

melody2 = [
    48 0.6 0.5; 50 0.6 0.5; 52 0.6 0.5; 53 0.6 0.5;
    55 0.6 1.0; 53 0.6 0.5; 52 0.6 0.5; 50 0.6 1.0
    ];

% melody3 = [
%     48 0.5 0.5; 48 0.5 0.25; 48 0.5 0.25; 50 0.5 0.25;
%     50 0.5 0.25; 52 0.5 0.5; 50 0.5 0.25; 48 0.5 0.5;
%     45 0.5 0.5; 45 0.5 0.25; 47 0.5 0.25; 48 0.5 0.25;
%     50 0.5 0.25; 52 0.5 0.5; 50 0.5 0.25; 48 0.5 0.5;
%     45 0.5 1.0
%     ];

% melody4 = [
%     72 0.4 0.5; 76 0.4 0.5; 79 0.4 0.5; 72 0.4 0.5;
%     74 0.4 0.5; 77 0.4 0.5; 81 0.4 0.5; 74 0.4 0.5;
%     72 0.4 0.5; 76 0.4 0.5; 79 0.4 0.5; 72 0.4 0.5;
%     74 0.4 0.5; 77 0.4 0.5; 81 0.4 0.5; 74 0.4 0.5;
%     72 0.4 1.0
%     ];

% -------------------------------
% Initialize VoiceManager
% -------------------------------
vm.numVoices = 2;
for i = 1:vm.numVoices
    vm.voices(i) = struct('active', false, 'note', [], 'phase', 0, 'samplesRemaining', 0);
end

% Index trackers for each melody
noteIndex1 = 1; noteIndex2 = 1; noteIndex3 = 1; noteIndex4 = 1;

% -------------------------------
% Estimate total samples to preallocate
% -------------------------------
totalSamples = sum(melody1(:,3) + melody2(:,3)) * fs;
finalWave = zeros(1, ceil(totalSamples));  % preallocate

writePos = 1;  % track where to write in finalWave

% -------------------------------
% Main synthesis loop
% -------------------------------
while noteIndex1 <= size(melody1,1) || noteIndex2 <= size(melody2,1) || ...
        any([vm.voices.active])

    % Assign new notes if voices are free (same as before)
    if ~vm.voices(1).active && noteIndex1 <= size(melody1,1)
        vm.voices(1).active = true;
        vm.voices(1).note = melody1(noteIndex1,1:2);
        vm.voices(1).samplesRemaining = round(melody1(noteIndex1,3)*fs);
        fprintf('Voice 1 STARTED note %d\n', melody1(noteIndex1,1));
        noteIndex1 = noteIndex1 + 1;
    end
    if ~vm.voices(2).active && noteIndex2 <= size(melody2,1)
        vm.voices(2).active = true;
        vm.voices(2).note = melody2(noteIndex2,1:2);
        vm.voices(2).samplesRemaining = round(melody2(noteIndex2,3)*fs);
        fprintf('Voice 2 STARTED note %d\n', melody2(noteIndex2,1));
        noteIndex2 = noteIndex2 + 1;
    end


    % Generate mixed audio block from all voices
    [block, vm] = voicemanager(vm, waveType, fs, blockSize);

    % -------------------------------
    % Write block into preallocated finalWave
    % -------------------------------
    finalWave(writePos:writePos+length(block)-1) = block;
    writePos = writePos + length(block);
end

% Trim any extra zeros (if totalSamples overestimated)
finalWave = finalWave(1:writePos-1);

% -------------------------------
% Play the final waveform
% -------------------------------
sound(finalWave, fs);