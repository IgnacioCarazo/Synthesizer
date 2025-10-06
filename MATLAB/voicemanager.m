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
%                  adsr             - ADSR parameters (attack, decay, sustain, release)
%                  env              - envelope state struct (see adsr_envelope)
%   waveType   - String specifying waveform: 'sine', 'triangle', 'square', 'saw'
%   fs         - Sampling rate in Hz
%   blockSize  - Number of samples to generate for this block
%
% Outputs:
%   blockOutput - Row vector containing the mixed samples of all active voices
%   vm          - updated VoiceManager structure (voice states updated)
% ---------------------------------------------------------------
fprintf('[%0.3f s] Entering <voice-manager>\n', toc);
% Initialize the output block with zeros
blockOutput = zeros(1, blockSize);

% Loop over all voices
for v = 1:vm.numVoices
    if vm.voices(v).active
        % Determine number of samples to generate for this voice this pass
        nSamples = min(blockSize, vm.voices(v).samplesRemaining);

        if strcmp(vm.voices(v).env.stage, 'release')
            nSamples = blockSize;  % generate full block for release
        else
            nSamples = min(blockSize, vm.voices(v).samplesRemaining);
        end

        % Call the voice to generate its block and update its internal state
        [noteBlock, voiceUpdated] = voice(vm.voices(v), waveType, fs, nSamples);

        % Store back the updated voice state
        vm.voices(v) = voiceUpdated;

        % Mix the voice block into the output buffer
        blockOutput(1:nSamples) = blockOutput(1:nSamples) + noteBlock;

        % Update remaining samples for the note (musical note length)
        if ~strcmp(vm.voices(v).env.stage, 'release')
            vm.voices(v).samplesRemaining = vm.voices(v).samplesRemaining - nSamples;
        end
        fprintf('Voice %d: samplesRemaining=%d, stage=%s\n', v, vm.voices(v).samplesRemaining, vm.voices(v).env.stage);

        % --- Handle release stage ---
        if vm.voices(v).samplesRemaining <= 0 && ~strcmp(vm.voices(v).env.stage, 'release')
            rel = vm.voices(v).adsr.release;
            if rel > 0
                vm.voices(v).env.stage = 'release';
                vm.voices(v).env.stageSampleCounter = 0;
                vm.voices(v).env.releaseStartLevel = vm.voices(v).env.level;
                vm.voices(v).samplesRemaining = ceil(rel * fs); % set samplesRemaining for release phase
                fprintf('[%0.3f s] Voice %d START RELEASE note %d\n', toc, v, vm.voices(v).note(1));
            else
                vm.voices(v).env.stage = 'idle';
                vm.voices(v).active = false;
                fprintf('[%0.3f s] Voice %d FINISHED note %d (no release)\n', toc, v, vm.voices(v).note(1));
            end
        end

        % --- Handle envelope idle (release done) ---
        if strcmp(vm.voices(v).env.stage, 'idle') && vm.voices(v).active
            vm.voices(v).active = false;
            fprintf('[%0.3f s] Voice %d FINISHED note %d (release done)\n', toc, v, vm.voices(v).note(1));
        end
    end
end
fprintf('[%0.3f s] Exiting <voice-manager>\n', toc);
end
