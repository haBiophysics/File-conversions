%%

function out = xwlc(p,y)
	kT = 4.02; %pN*nm
	B = -4;
	S = 1200; % elastic stretch modulus = 1200 pN
	
	D = B^2 + 4/27*(3-(4*y*p(2)/kT)).^3;
	isReal = (D>0);
	
	u = nthroot(0.5*(-B + sqrt(isReal.*D)),3) - nthroot(0.5*(B + sqrt(isReal.*D)),3) + 2*real( (0.5*(-B + sqrt(~isReal .* D)) ).^(1/3) );
		
	out = p(1) * ( 1 + y/S - 1./u); % for F < 10 pN
end
