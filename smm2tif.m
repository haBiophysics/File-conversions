%% Tunc Kayikcioglu, 2016
% Ha Lab, Johns Hopkins University

function film = smm2tif ( filename )
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
	nframes = uint32( ( num_bytes - headersize ) / num_bytes_per_frame;
	
	% Filename will be the same except the extension.
	% If there is a file name conflict, the new file will be generated as name(1), name(2).tif ...
	outfilename = [filename(1:end-4) '.tif'];
	k = 1;
	while exist(outfilename) ~= 0
		outfilename = [filename(1:end-4) '(' num2str(k) ').tif'];
		k = k+1;
	end
	
	for i=1:1:nframes
		switch bpp
			case 1
				framedata = fread( file, num_pixels_per_frame, 'uint8' );
				imwrite(uint8(reshape(framedata, framew, frameh)), outfilename, 'WriteMode', 'append');
			case 2
				framedata = fread( file, num_pixels_per_frame, 'uint16' );
				imwrite(uint16(reshape(framedata, framew, frameh)), outfilename, 'WriteMode', 'append');
			case 4
				framedata = fread( file, num_pixels_per_frame, 'uint32' );
				imwrite(uint32(reshape(framedata, framew, frameh)), outfilename, 'WriteMode', 'append');
			otherwise
			  error([num2str(bpp) '-bit images not supported.']);
		end
	end
return
