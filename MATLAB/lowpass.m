function bank = lowpass(~, fs, cutoffList)

if nargin < 3 || isempty(cutoffList)
    cutoffList = logspace(log10(20), log10(fs/2*0.95), 50);
end

bank = struct('cutoff', {}, 'b', {}, 'a', {}, 'state', {});

for k = 1:length(cutoffList)
    fc = cutoffList(k);
    RC = 1/(2*pi*fc);
    dt = 1/fs;
    alpha = dt/(RC + dt);
    bank(k).cutoff = fc;
    bank(k).b = alpha;
    bank(k).a = 1 - alpha;
    bank(k).state = 0;
end
end
