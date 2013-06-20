classdef la
    %la Summary of this class goes here
    %   Detailed explanation goes here
    
    properties
    end
    
    methods(Access = public, Static = true)
        
        function run
            la.plot_synth_opt(242.5, 'p', 1, 250.0, 'p', -1);
            %{
            figure;
            la.plot_synth(242.5, 'p', 1);
            figure;
            la.plot_synth(255.0, 'c', -1);
            figure;
            la.plot_synth(250.0, 'c', -1);
            %}
            %la.plot_synth(260, 'p', 1);
            %la.plot_synth(270, 'p', 1);
        end
        
        function plot_synth_opt(strike1, callput1, multip1, strike2, callput2, multip2)
            [yg1, ygatm1] = la.optgrid(la.ref, la.xgref, strike1, la.rfrate, la.mat, la.vol, callput1);
            [yg2, ygatm2] = la.optgrid(la.ref, la.xgref, strike2, la.rfrate, la.mat, la.vol, callput2);
            yg = (yg1 - ygatm1) * multip1 +  (yg2 - ygatm2) * multip2;
            plot(la.xgref, yg, la.xgref, (la.xgref - la.ref) * 1);
        end
        
        function plot_synth(strike, callput, futslope)
            [yg, ygatm] = la.optgrid(la.ref, la.xgref, strike, la.rfrate, la.mat, la.vol, callput);
            yg_hedged_fut = la.synth(la.xgref, yg, futslope, ygatm);
            plot(la.xgref, yg_hedged_fut, la.xgref, (la.xgref - la.ref) * futslope);
        end
        
        function ret = synth(xg, yg, slope, atm)
            ret = yg - atm;
            fut = (xg - la.ref) * slope;
            ret = ret + fut;
        end
        
        function [yg, atm] = optgrid(ref, refxg, strike, rfrate, mat, vol, cp)
            [c, p] = blkprice(refxg, strike, rfrate, mat, vol);
            [catm, patm] = blkprice(ref, strike, rfrate, mat, vol);
            if (cp == 'p')
                yg = p;
                atm = patm;
            else
                yg = c;
                atm = catm;
            end
        end
        
        function ret = vol
            ret = 0.17;
        end
        
        function ret = ref
            ret = 250.0;
        end
        
        function ret = rfrate
            ret = 0.03;
        end
        
        function ret = mat
            ret = 0.15;
        end
        
        function ret = xg(ref)
            grid = [0.98:0.001:1.02];
            %grid = [0.85:0.01:1.15];
            ret = grid * ref;
        end
        
        function ret = xgref
            ret = la.xg(la.ref);
        end
        
        
    end
    
end

