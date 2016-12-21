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
	
	% Read the pixels from input file and organized in a 4D tensor (x,y,1 for grayscale, numFrames)
	stack = reshape(uint8(fread(file, inf, 'uint8')), xdim, ydim, 1, numFrames);
	fclose(file);
	
	if nargin == 1
		% Output a tif stack
		outname = [filename(1:end-4) '.tif'];
		if exist(outname)
			ans = input([outname ' already exists. Overwrite? <Y/N> '], 's');
			if ans ~= 'y' && ans ~= 'Y'
				fprintf('Write operation aborted.\n');
				return;
			else
				delete(outname);
				fprintf('Overwritten.\n');
			end
		end
		imwrite(stack, outname);
	else
		if strcmp(opt, 'tif') || strcmp(opt, '-dtif')
			% Individual frames recorded as separate tif images.
			for k = 1:1:size(stack,3)
				imwrite(stack(:,:,k), [filename(1:end-4) '_' num2str(k) '.tif']);
			end
			return
		elseif strcmp(opt, '-none')
			% Do not record any output, convert only.
			% We reduce the dimension of the output from 4 to 3: "mxnx1xr"
			stack = squeeze(stack);
			return
		else
			% Should not reach here, unknown option.	
			error(sprintf('Undefined option %s.\n', opt));
		end
	end
end
