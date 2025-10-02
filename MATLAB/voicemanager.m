function [blockOutput, vm] = voicemanager(vm, waveType, fs, blockSize)
% ---------------------------------------------------------------
% VoiceManager - Generate a block of samples for all active voices
% ---------------------------------------------------------------
%
% This function processes a single block of audio for all active voices
% in the VoiceManager structure. Each voice generates its own waveform
% using the specified wave type, and the outputs are summed to produce
% the final audio block. Phase tracking ensures smooth waveform continuity
% between successive blocks.
%
% Inputs:
%   vm         - Structure containing all voices
%                Each voice should have fields:
%                  active           - Boolean, true if the voice is currently playing
%                  note             - [MIDI_number, velocity]
%                  phase            - Current phase of the waveform [0,1)
%                  samplesRemaining - Number of samples left to play
%   waveType   - String specifying waveform: 'sine', 'triangle', 'square', 'saw'
%   fs         - Sampling rate in Hz
%   blockSize  - Number of samples to generate for this block
%
% Outputs:
%   blockOutput - Row vector containing the mixed samples of all active voices
%

% ---------------------------------------------------------------

% Initialize the output block with zeros
blockOutput = zeros(1, blockSize);


% Loop over all voices
for v = 1:vm.numVoices
    if vm.voices(v).active
        % Determine number of samples to generate for this voice
        nSamples = min(blockSize, vm.voices(v).samplesRemaining);

        % Generate waveform block for this voice
        [noteBlock, vm.voices(v).phase] = voice( ...
            vm.voices(v).note, waveType, fs, vm.voices(v).phase, nSamples);

        % Mix this voice's samples into the output block
        blockOutput(1:nSamples) = blockOutput(1:nSamples) + noteBlock;

        % Update remaining samples
        vm.voices(v).samplesRemaining = vm.voices(v).samplesRemaining - nSamples;

        % Deactivate the voice if it has finished
        if vm.voices(v).samplesRemaining <= 0
            vm.voices(v).active = false;

        end

    end
end

end
