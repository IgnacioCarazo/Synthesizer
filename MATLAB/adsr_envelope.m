function [envSeg, voice] = adsr_envelope(voice, nSamples, fs)
% ADSR_ENVELOPE - Produce an ADSR envelope segment for a voice and update voice.env
% -------------------------------------------------------------------------
% Inputs:
%   voice    - voice struct, must contain:
%                voice.adsr.attack   (seconds)
%                voice.adsr.decay    (seconds)
%                voice.adsr.sustain  (0..1)
%                voice.adsr.release  (seconds)
%                voice.env.stage     ('idle','attack','decay','sustain','release')
%                voice.env.stageSampleCounter (integer, samples consumed in current stage)
%                voice.env.level     (current envelope value 0..1)
%                voice.env.releaseStartLevel (optional, used for release interpolation)
%   nSamples - number of envelope samples to produce
%   fs       - sample rate (Hz)
%
% Outputs:
%   envSeg   - 1 x nSamples envelope vector (values 0..1)
%   voice    - updated voice struct with modified voice.env fields
%
% Notes:
%   - The function is block-friendly: it advances the state machine across the block
%     and updates the per-voice counters/state so the next call will continue where it left off.
% -------------------------------------------------------------------------
fprintf('[%0.3f s] Entering <envelope>\n', toc);
% Safety defaults if fields missing
if ~isfield(voice, 'adsr') || isempty(voice.adsr)
    % default gentle ADSR
    voice.adsr.attack  = 0.01;
    voice.adsr.decay   = 0.1;
    voice.adsr.sustain = 0.8;
    voice.adsr.release = 0.2;
end

if ~isfield(voice, 'env') || isempty(voice.env)
    voice.env.stage = 'idle';
    voice.env.level = 0;
    voice.env.stageSampleCounter = 0;
    voice.env.releaseStartLevel = 0;
end

adsr = voice.adsr;
env = voice.env;

envSeg = zeros(1, nSamples);

for i = 1:nSamples
    switch env.stage
        case 'idle'
            v = 0;
            % remain idle until someone triggers attack externally
        case 'attack'
            if adsr.attack <= 0
                % instantaneous attack -> jump to decay immediately
                env.stage = 'decay';
                env.stageSampleCounter = 0;
                v = 1;
            else
                attackSamples = max(1, round(adsr.attack * fs));
                env.stageSampleCounter = env.stageSampleCounter + 1;
                v = env.stageSampleCounter / attackSamples;
                if env.stageSampleCounter >= attackSamples
                    env.stage = 'decay';
                    env.stageSampleCounter = 0;
                    v = 1;
                end
            end
        case 'decay'
            if adsr.decay <= 0
                env.stage = 'sustain';
                env.stageSampleCounter = 0;
                v = adsr.sustain;
            else
                decaySamples = max(1, round(adsr.decay * fs));
                env.stageSampleCounter = env.stageSampleCounter + 1;
                % linear decay from 1 -> sustain
                v = 1 - (1 - adsr.sustain) * (env.stageSampleCounter / decaySamples);
                if env.stageSampleCounter >= decaySamples
                    env.stage = 'sustain';
                    env.stageSampleCounter = 0;
                    v = adsr.sustain;
                end
            end
        case 'sustain'
            v = adsr.sustain;
            % Remain in sustain until an external note-off sets stage to 'release'
        case 'release'
            if adsr.release <= 0
                % instantaneous release
                v = 0;
                env.stage = 'idle';
                env.stageSampleCounter = 0;
            else
                releaseSamples = max(1, round(adsr.release * fs));
                env.stageSampleCounter = env.stageSampleCounter + 1;
                startLevel = env.releaseStartLevel;
                % linear release from startLevel -> 0
                v = max(0, startLevel * (1 - env.stageSampleCounter / releaseSamples));
                if env.stageSampleCounter >= releaseSamples || v <= 1e-6
                    v = 0;
                    env.stage = 'idle';
                    env.stageSampleCounter = 0;
                end
            end
        otherwise
            % safety fallback
            v = 0;
            env.stage = 'idle';
            env.stageSampleCounter = 0;
    end

    % store and update level
    env.level = v;
    envSeg(i) = v;
end

% write back updated env
voice.env = env;
fprintf('[%0.3f s] Exiting <envelope>\n', toc);
end
