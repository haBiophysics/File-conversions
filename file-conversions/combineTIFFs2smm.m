%% Tunc Kayikcioglu, 2017
% Ha Lab, Johns Hopkins University


function combineTIFFs2smm (infilename1, infilename2, outfilename);
	tags = imfinfo(infilename1);
	nframes1 = length(tags);
	framew1 = tags(1).Width;
	frameh1 = tags(1).Height;
	bitDepth1 = tags(1).BitDepth;
		
	tags = imfinfo(infilename2);
	nframes2 = length(tags);
	framew2 = tags(1).Width;
	frameh2 = tags(1).Height;
	bitDepth2 = tags(1).BitDepth;
	
	if framew1 ~= framew2 || frameh1 ~= frameh2
		error('Cannot concatenate. Image dimensions do not match.\n')
	end
	
	if bitDepth1 ~= bitDepth2
		error('Cannot concatenate. Bit depths of images do not match.\n');
	end
	
	% Do not know what to do with these.
	background = 0;
	data_scaler = 1;
	framecycle = 1;
	
	file = fopen(outfilename, 'w');
	fwrite(file, framew1, 'uint16');
	fwrite(file, frameh1, 'uint16');
	fwrite(file, bitDepth1/8, 'uint8'); % bytes per pixel, typically 1 or 2
	fwrite(file, background, 'uint32');
	fwrite(file, data_scaler, 'uint32');
	fwrite(file, framecycle, 'float32');
	
	switch bitDepth1
		case 8
			precision = 'uint8';
		case 16
			precision = 'uint16';
		case 32
			precision = 'uint32';
		otherwise
			error([num2str(bpp) '-bit images not supported.']);	
	end	
	
	for i=1:1:nframes1
		frame = imread(infilename1,i);
		fwrite(file, frame, precision);
	end

	for i=1:1:nframes2
		frame = imread(infilename2,i);
		fwrite(file, frame, precision);
	end
	
	fclose(file);
end
