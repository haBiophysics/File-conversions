% p(1): L
% p(2): kT/P
% p(3): offset

% g(x,F) = 0, so this should ideally be a vector of zeros.
function out = eWLC (p, q)
	x = q(1,:);
	F = q(2,:);
	Ko = 1200;

	% I implemented the two different asymptotic limits on Wikipedia WLC page and take the value of whatever regime we are in.
	lowF = (F<10);
	highF = ~lowF;	
	
 	out = highF .* ( 1 - 0.5*sqrt(p(2)./F) + F/Ko - x/p(1) ) + lowF .* ( 0.25*(1-x/p(1)+F/Ko).^(-2) - 0.25 + x/p(1) - F/Ko - F/p(2) );
end
