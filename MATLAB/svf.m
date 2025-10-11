function [y, state] = svf(x, fc, Q, fs, state, mode)

if nargin < 6
    mode = 'lowpass';
end
if nargin < 5 || isempty(state)
    state = struct('s1',0,'s2',0);
end

g = tan(pi * fc / fs);
k = 1 / Q;

y = zeros(size(x));

for n = 1:length(x)
    v0 = x(n);
    v1 = (v0 - k*state.s1 - state.s2) / (1 + g*(g + k));
    v2 = g * v1 + state.s1;  % band-pass
    v3 = g * v2 + state.s2;  % low-pass
    state.s1 = 2*v2 - state.s1;
    state.s2 = 2*v3 - state.s2;

    switch mode
        case 'lowpass'
            y(n) = v3;
        case 'bandpass'
            y(n) = v2;
        case 'highpass'
            y(n) = v0 - k*v2 - v3;
    end
end
end
