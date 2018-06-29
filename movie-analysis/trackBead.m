% Tracks a bead through an SMM movie and deduces the peak position and spread at each frame.

function status = trackBead( smmdir, smmname );
	fclose('all');
	addpath('..\LOG\');
	addpath('..\SMM\');

	if ( nargin < -2 )
		smmdir = input('SMM Directory: ', 's');
		smmname = input('SMM Filename: ', 's');
	end

	% Import SMM file
	smmdir = 'E:\Lab\L108 TJ Ha Lab\Project\RPA\2018.06.22 Height cali2\sBead1-2\';
	smmname = 'film1.smm';
	[movie, background, scaler] = smm2tif([smmdir smmname], 'nofile');
	nframes = size(movie,3);
	avgframe = mean(movie,3);
	framew = size(movie,1);
	frameh = size(movie,2);


	flogname = [ smmname(1:(end-3)) 'log' ];
	flog = ReadLOG( smmdir, flogname );
	if ( ~flog.is_open )
		error(sprintf('LOG file [ %s\\%s ] open failed.\n', smmdir, flogname) );
	end

	figure(1);
	clf(1);
	cmap = colormap(SMMLoad8bitColorMap());

	image( uint8( 255 * ( avgframe - background )/scaler ) );
	colormap(cmap);
	title( 'Avg Frame' );
	axis off;
	axis image;

	flag = true;
	while flag
		[ gx, gy ] = ginput;
		if length(gx) > 1
			gx = gx(end);
			gy = gy(end);
		end
		
		if ( gx<1 || gx>framew || gy < 1 || gy>frameh)
			fprintf('Selected spots out of bounds, try again.\n');
		else
			flag = false;
		end
	end

	beadname = [ smmname(1:(end-4)) '.bead' ];
	fid = fopen( [ smmdir '\' beadname ], 'w' );
	if (fid == -1)
		error('Failed to open file to export');
	end
	
	fprintf( fid, 'FrameNo \t FrameNo \t P1(bg) \t P2(I) \t P3(sx) \t P2(I) \t P3(sx) \t P4(x) \t P6(y) \t R \r\n' );		   
	
	% Initial guesses for parameters
	radius = 10; % px, guess on spot size
	p0 = zeros(5,1);
    	p0(1) = median(avgframe); % Background
	p0(2) = max(max(avgframe)); % amplitude
	p0(3) = 1/2*radius^2; % inverse spread along x
	p0(4) = gx; % x pos
	p0(5) = 1/2*radius^2; % inverse spread along y
	p0(6) = gy; % y pos
	
	lowerBound = [0, 0, 1/(2*framew^2), 1, 1/(2*frameh^2), 1];
	upperBound = [1e4, 1e4, 1, framew, 1 frameh];
	options = optimset('Display', 'off');
   
   	[xx,yy] = meshgrid();
   	gridLinear = [reshape(xx,1,[]); reshape(yy,1,[])];
	
	fprintf( 1, 'SMM [ %s\\%s ]\n', smmdir, smmname );
	for i = 1:nframes
		frame = movie(:,:,i);
		bg = sum(sum(frame))/(framew*frameh);
		frame = abs(frame - bg);
		frameLinear = reshape(frame, 1, []);
		
		params = lsqcurvefit(@Gaussian2D, p0, gridLinear, frameLinear, lowerBound, upperBound, options);
		R = 0.5*( 3*sqrt(0.5/params(3)) + 3*sqrt(0.5/params(5)) );

		% Update initial guesses based on the current position
		p0 = params;
		
		% Calculate the moment of intertia to account for diffraction rings.
		r2 = (xx-params(4)).^2 + (yy-params(6)).^2;
		I = sum(sum(r2 .* frame));
		
		fprintf( fid, '%d \t %d \t %.6g \t %.6g \t %.6g \t %.6g \t %.6g \t %.3f \t %.3f \t %.3f \r\n', i, flog.serverIdx-1 + i, params(:), I );
    end
    fclose(fid);
    status = 0;
return;


function z = Gaussian2D( p, xy );
	n = length(xy);
	x = xy(:,1);
	y = xy(:,2);
	
	z = p(1) + p(2) * exp( -p(3) * (x-p(4)).^2 - p(5)*(y-p(6)).^2 );
return;


