% Imports a pma movie and plots the fetches at the begining and end of the movie.
% Displays the spots in an inverted colour scheme.

files = dir('./*.pma');

for i=1:1:length(files)
	filename = files(i).name;
	attributes = dir(filename);
	fileSize = attributes.bytes;
	
	file = fopen(filename, 'r');
	xdim = fread(file, 1, 'int16');
	ydim = fread(file, 1, 'int16');
	numFrames = (fileSize-4)/(xdim*ydim);
	
	% Read the pixels from input file and organized in a 4D tensor (x,y,1 for grayscale, numFrames)
	stack = reshape(uint8(fread(file, inf, 'uint8')), xdim, ydim, 1, numFrames);
	fclose(file);
	
	begin = uint8(mean(stack(:,:,1:10), 3)');
	final = uint8(mean(stack(:,:,end-9:end), 3)');
	
	threshold = 20;
	begin (begin> threshold) += 100;
	final (final> threshold) += 100;
	
	imwrite(255-begin, [filename(1:end-4) '_begin.jpg']);
	imwrite(255-final, [filename(1:end-4) '_end.jpg']);
end		
