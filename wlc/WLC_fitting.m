%% Fits to the WLC equation.
% Tunc Kayikcioglu, 2016

% Comment out for MATLAB
pkg load optim;


% Provide your force-extension data here.
% x : extension in um
% y : force in pN
file = fopen('./Downloads/Bead2-4_X.dat');
for i=1:1:3
	fgetl(file);
end
z = fscanf(file, '%f', [7,inf]);
x = z(1,:);
y = z(end,:);


wlcF = @(p,x) p(1) + p(2) * ( 0.25*(1-x/p(3)).^(-2) -0.25 + x/p(3) );
p_initial = [0.1; 0.1; 1e2];

% Uncomment below for MATLAB
p = lsqcurvefit(wlcF, p, x, y);

% Comment below for Octave
p = nonlin_curvefit (wlcF, p_initial, x, y);


yfit = feval (wlcF, p,x);

set(0,'DefaultAxesFontSize',18);
plot(x, y, 'b*', x, yfit, 'r'); 
xlabel('Extension (um)');
ylabel('Force (pN)');

kT = 4.02; %pN nm
persistenceLength = kT/p(2) % nm
contourLength = p(3) % um
forceOffset = p(1) % pN
