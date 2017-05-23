%%

function out = tempsmooth (inputname, outputname)
	image = readtiff(inputname);

	sigma = 100;
	kernel = ones(1,1,2*sigma+1);
	for i=1:1:size(kernel,3)
		kernel(1,1,i)  = e.^(-(i-sigma-1)^2/(2*sigma^2));
	end
	kernel = kernel / sum(sum(sum(kernel)));

	out = convn(image, kernel, 'same');

%	for i=1:1:size(image,3)
%		imshow(out(:,:,i));
%		pause(0.2);
%	end

	imwrite(out, outputname);
return
