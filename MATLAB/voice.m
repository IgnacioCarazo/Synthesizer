function [noteBlock, voiceOut] = voice(voiceIn, waveType, fs, nSamples)
% SYNTHVOICE - Generate waveform for a single note (block) and apply ADSR
%
% Inputs:
%   voiceIn   - voice struct containing at least:
%                 .note  = [MIDI_number, velocity]
%                 .phase = current phase [0,1)
%                 .adsr  = struct('attack',..,'decay',..,'sustain',..,'release',..)
%                 .env   = envelope state struct (see adsr_envelope)
%   waveType  - 'sine','triangle','square','saw'
%   fs        - sampling rate (Hz)
%   nSamples  - number of samples to generate in this block
%
% Outputs:
%   noteBlock - 1 x nSamples vector of samples (oscillator * ADSR)
%   voiceOut  - updated voice struct (phase, env, ... updated)
%
% Notes:
%   - This function now accepts and returns the full voice struct so per-voice
%     state (ADSR, phase) can persist across blocks.
% -------------------------------------------------------------------------
fprintf('[%0.3f s] Entering <voice>\n', toc);
% Default output
noteBlock = zeros(1, nSamples);
voiceOut = voiceIn;

% validate presence of note
if isempty(voiceIn) || ~isfield(voiceIn, 'note') || isempty(voiceIn.note)
    % nothing to do, return zeros and unchanged voice
    return;
end

% Extract frequency and amplitude from MIDI note + velocity
freq = 440 * 2.^((voiceIn.note(1) - 69)/12);
amp  = voiceIn.note(2);

% Generate raw oscillator block (phase-aware)
[oscBlock, newPhase] = oscillator(waveType, freq, amp, fs, voiceIn.phase, nSamples);

% Update phase in output voice
voiceOut.phase = newPhase;

% Compute ADSR envelope for this block and update voice env state
[envSeg, voiceOut] = adsr_envelope(voiceOut, nSamples, fs);

% Apply envelope to oscillator output
% Ensure oscBlock is same length as envSeg
if length(oscBlock) < length(envSeg)
    oscBlock(end+1:length(envSeg)) = 0;
elseif length(envSeg) < length(oscBlock)
    envSeg(end+1:length(oscBlock)) = 0;
end

noteBlock = oscBlock .* envSeg;

[noteBlock, voiceOut] = filter_block(noteBlock, voiceOut);
fprintf('[%0.3f s] Exiting <voice>\n', toc);
end
