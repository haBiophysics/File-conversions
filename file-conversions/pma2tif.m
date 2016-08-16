%% Tunc Kayikcioglu, 2016
% Ha Lab, Johns Hopkins University

% Converts *.pma file to stacked *.tif
% USAGE:
% No option provided --> tif stack
% 'tif' --> separate files for each frame

function stack = pma2tif (filename, opt)
	attributes = dir(filename);
	fileSize = attributes.bytes;

	file = fopen(filename, 'r');
	xdim = fread(file, 1, 'int16');
	ydim = fread(file, 1, 'int16');
	numFrames = (fileSize-4)/(xdim*ydim);

	stack = zeros(xdim,ydim,numFrames,'uint8');
	for i=1:1:numFrames
		frame = fread(file,[xdim,ydim],'uint8');
		stack(:,:,i) = frame';
	end
	fclose(file);
	
	if nargin == 1
		outname = [filename(1:end-4) '.tif'];
		if exist(outname)
			ans = input([outname ' already exists. Overwrite? <Y/N> '], 's');
			if ans == 'n' || ans == 'N'
				fprintf('Write operation aborted.\n');
				return;
			else
				delete(outname);
			end
		end
		for k = 1:1:size(stack,3)
			imwrite(stack(:,:,k), outname, 'writemode', 'append');
		end
	elseif strcmp(opt, 'tif') || strcmp(opt, '-dtif')
		for k = 1:1:size(stack,3)
			imwrite(stack(:,:,k), [filename(1:end-4) '_' num2str(k) '.tif']);
		end
	else
		error(sprintf('Undefined option %s.\n', opt));
	end
end
