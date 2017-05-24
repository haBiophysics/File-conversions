%% Function to perform smoothing on tiff movies both in spatial and temporal domains.
% A gaussian smoothing is applied also between consecutive frames.

% Tunc Kayikcioglu 2017

 function out = spatiotemporalSmoothing (inputname, outputname)
	image = readtiff(inputname);

	sigmat = 10; % Gaussian spread for temporal smoothing
	sigmax = 10; % Gaussian spread for spatial smoothing
	
	kernel = zeros(2*sigmax+1,2*sigmax+1,2*sigmat+1);
	gauss2d = fspecial('gaussian', [size(kernel,1) size(kernel,2)], sigmax);
	gauss1d = fspecial('gaussian', [size(kernel,3),1], sigmat);
 	for i=1:1:size(kernel,3)
		kernel(:,:,i)  = gauss2d * gauss1d(i);
	end
	kernel = kernel / sum(sum(sum(kernel)));

	out = convn(image, kernel, 'same');

	% Enable below to show the frames mimicking a movie.
	if false
		for i=1:1:size(image,3)
			imshow(out(:,:,i));
			pause(0.1);
		end
	end

	imwrite(out, outputname);
return
