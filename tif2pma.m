%% Tunc Kayikcioglu, 2016
% Ha Lab, Johns Hopkins University

% Converts stacked *.tif file to *.pma

function stack = tif2pma (filename)
	if nargin == 0
		error('No input file specified\n');
	end
	
	tags = imfinfo(filename);
	numFrames = length(tags);
	width = tags(1).Width;
	height = tags(1).Height;
	
	file = fopen([filename(1:end-4) '.pma'], 'w');
	fwrite(file, width,'uint16');
	fwrite(file, height,'uint16');
	for i=1:1:numFrames
		x = imread(filename,1);
		fwrite(file, x', 'uint8');
	end
	fclose(file);
end
