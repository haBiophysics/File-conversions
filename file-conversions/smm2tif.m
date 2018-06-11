% A script to import SMM movie files to matlab. If requested, the movie is converted to a TIF file. Else, the gray data is returned as a 3-D tensor.

function outMovie = smm2tif ( filename, opt )
	file = fopen(filename, 'r');
	if file == -1
		error(['File ' filename ' could not be read.']);
	end
	
	framew = fread( file, 1, 'uint16' );
	frameh = fread( file, 1, 'uint16' );
	bpp = fread( file, 1, 'uint8' ); % bytes per pixel, typically 1 or 2
	background = fread( file, 1, 'uint32' );
	data_scaler = fread( file, 1, 'uint32' );
	framecycle = fread( file, 1, 'float32' );
	headersize = 2+2+1+4+4+4; % bytes that do not describe any pixels
	
  	num_pixels_per_frame = framew*frameh;
	num_bytes = dir(filename).bytes;
	num_bytes_per_frame = bpp*num_pixels_per_frame;
	nframes = uint32( ( num_bytes - headersize ) / num_bytes_per_frame );
	
	
	bitdepthstring = '';
	switch bpp
		case 1
			bitdepthstring = 'uint8';
		case 2
			bitdepthstring = 'uint16';
		case 4
			bitdepthstring = 'uint32';
		otherwise
			error([num2str(bpp) '-bit smm images not supported.']);
	end
	
	outMovie = zeros(framew, frameh, nframes);
	for i=1:1:nframes
		framedata = fread( file, num_pixels_per_frame, bithdepthstring );
		outMovie(:,:,i) = reshape(framedata, framew, frameh);
	end
	
	
	% Generate a tif file output containing the frames of the SMM movie as different frames.
	if nargin == 1 || opt ~= 'nofile'
		% Filename will be the same except the extension.
		% If there is a file name conflict, the new file will be generated as name(1), name(2).tif ...
		outfilename = [filename(1:end-4) '.tif'];
		k = 1;
		while exist(outfilename) ~= 0
			outfilename = [filename(1:end-4) '(' num2str(k) ').tif'];
			k = k+1;
		end
	
		switch bpp
			case 1
				output = uint8(outMovie);
			case 2
				output = uint16(outMovie);
			case 4
				output = uint32(outMovie);
			otherwise
				error([num2str(bpp) '-bit smm images not supported.']);
		end
	
		for i=1:1:nframes
			imwrite(output(:,:,i), outfilename, 'WriteMode', 'append');
		end
	end
	
return
