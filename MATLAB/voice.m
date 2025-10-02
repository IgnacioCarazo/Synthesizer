function [noteBlock, newPhase] = voice(note, waveType, fs, initPhase, nSamples)
% SYNTHVOICE - Generate waveform for a single note
%
% Inputs:
%   note       - [MIDI_number, velocity]
%   waveType   - 'sine','triangle','square','saw'
%   fs         - sampling rate
%   initPhase  - fase inicial [0,1)
%   nSamples   - cantidad de samples a generar
%
% Outputs:
%   noteBlock  - bloque de samples generado
%   newPhase   - fase final


freq = 440 * 2.^((note(1) - 69)/12);
amp = note(2);

% Llamamos al oscilador para generar bloque
[noteBlock, newPhase] = oscillator(waveType, freq, amp, fs, initPhase, nSamples);

% Si se piden menos samples que duration, oscillator se ajusta por duración
noteBlock = noteBlock(1:nSamples);
end
