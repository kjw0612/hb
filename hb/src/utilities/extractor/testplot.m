function [ output_args ] = testplot( input_args )
    %PLOT Summary of this function goes here
    %   Detailed explanation goes here
    [m, txt] = xlsread('plotfile.csv');
    for i=2:size(txt,1)
        plot(m(1,:), m(i,:), 'Color', [0.2 0.2 0.2] + [rand()/2 rand()/2 rand()/2]);
        hold on;
    end
    legend(txt(2:end));
    hold off;
end

