% ---------------------------------------------------------------
% Example usage of a wave as part of an oscillator
% ---------------------------------------------------------------

% Oscillator parameters
oscParams.fs = 44100;        % sampling rate

% -------------------------------
% Happy Birthday en MIDI with velocities
% -------------------------------
% Each row: [MIDI note number, velocity (0 to 1)]
midiNotesC4 = [60, 0.8; 60, 0.75; 62, 0.9; 60, 0.8; 65, 0.85; 64, 0.8; ...
    60, 0.8; 60, 0.75; 62, 0.9; 60, 0.8; 67, 0.85; 65, 0.8; ...
    60, 0.8; 60, 0.75; 72, 0.9; 69, 0.85; 65, 0.8; 64, 0.8; 62, 0.75; ...
    70, 0.85; 70, 0.75; 69, 0.85; 65, 0.8; 67, 0.85; 65, 0.8];

midiNotes = midiNotesC4;

% MIDI to frequency (Hz)
freqs = 440 * 2.^((midiNotes(:,1) - 69)/12);

% Initialize array to store final waveform
finalNotes = [];

% Initialize phase tracking
phase = 0;

% Generate waveform note by note
for i = 1:length(freqs)
    noteFreq = freqs(i);
    noteVelocity = midiNotes(i, 2);

    % Call wave function
    [waveforms, noteCycle, phaseIncrement] = triangle(noteFreq, oscParams.fs, noteVelocity);

    % Generate the note duration using phase increment
    noteLengthSec = 0.5;  % example: 0.5 s per note
    NsNote = round(noteLengthSec * oscParams.fs);

    % Allocate array for note
    noteWave = zeros(1, NsNote);

    % Fill note with repeated single-cycle waveform, tracking phase
    for n = 1:NsNote
        % Compute index in cycle using phase
        idx = floor(phase * length(noteCycle)) + 1;
        noteWave(n) = noteCycle(idx);

        % Increment phase
        phase = phase + phaseIncrement;
        if phase >= 1
            phase = phase - 1;  % wrap phase
        end
    end

    % Append note to final waveform
    finalNotes = [finalNotes, noteWave];
end

% Play the sequence
sound(finalNotes, oscParams.fs);
