function [block, newPhase] = oscillator(waveType, freq, amp, fs, initPhase, blockSize)
% ---------------------------------------------------------------
% Generate a block of samples for a single voice using existing wave functions
% ---------------------------------------------------------------
% Inputs:
%   waveType   - 'sine','triangle','square','saw'
%   freq       - note frequency (Hz)
%   amp        - amplitude
%   fs         - sampling rate
%   initPhase  - initial phase [0,1)
%   blockSize  - number of samples to generate
%
% Outputs:
%   block      - generated block of samples
%   newPhase   - phase at the end of the block
% ---------------------------------------------------------------

% Map waveType to function handle
switch lower(waveType)
    case 'sine'
        waveFunc = @sine;
    case 'triangle'
        waveFunc = @triangle;
    case 'square'
        waveFunc = @square;
    case 'saw'
        waveFunc = @saw;
    otherwise
        error('Unknown wave type');
end

% Generate single-cycle waveform
[~, noteCycle, phaseIncrement] = waveFunc(freq, fs, amp);

% Initialize block
block = zeros(1, blockSize);
phase = initPhase;

% Fill block using phase tracking
for n = 1:blockSize
    idx = floor(phase * length(noteCycle)) + 1;
    block(n) = noteCycle(idx);
    phase = phase + phaseIncrement;
    if phase >= 1
        phase = phase - 1;  % wrap phase
    end
end

newPhase = phase;

end
