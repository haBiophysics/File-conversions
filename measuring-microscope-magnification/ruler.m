% Tunc Kayikcioglu, 2016
% Ha Lab, Johns Hopkins University

%% Measures the distance between the bars in the input file
% Used for the quantification of the magnification along two axes of the microscope using a ruler slide
% >2 images of the ruler slide should be taken, in an alignment as paralleel to the x and y axes as possible.
% The code uses linear fitting to the bars, so slight deviations are tolerable.

pkg load statistics;
more off;

filename = cell;
filename{1} = 'x.tif';
filename{2} = 'y.tif';

% Uniform kernel to smooth out the input images
% The larger the kernel is, the more smoothing there will be.
kernelDim = 7;
kernel = 1/(kernelDim^2)*ones(kernelDim);

bins = 1:5:1100; % For x-y pixel indeces
parfor jj=1:1:length(filename)
	cfilename = filename{jj}
	xim = double(imread(cfilename))';
	if size(xim,3) ~= 1 % If input image is RGB, convert to grayscale
		xim = xim(:,:,1)*0.2989 + xim(:,:,2)*0.5870 + xim(:,:,3)*0.1140;
	end
	
	xmin = min(min(xim));
	xmax = max(max(xim));
	xthreshold = (3*xmin+xmax)/4; % 25th percentile
	smooth = conv2(xim,kernel, 'valid');
	xbool = (smooth < xthreshold); % if on a bar, 1; else 0
	
	% Retrieve the indeces of the pixels corresponding to the bars.
	% Randomly select a subset of all pixels to reduce working set size.
	[xpos, ypos] = find(xbool);
	chosen = (rand(length(xpos),1) > 0.8);
	xpos = xpos(chosen);
	ypos = ypos(chosen);
	
	xhist = histc(xpos, bins);
	yhist = histc(ypos, bins);
	if std(yhist) > std(xhist) % bars oriented along x, else along y.
		chist = yhist;
		cpos = ypos;
		rotflag = false;
	else
		chist = xhist;
		cpos = xpos;
		rotflag = true;
	end
	
	
	% Distinguish different bars from each other and classify all bar-points accordingly.
	[cmax,maxpos] = max(chist);
	absMax = cmax;
	centerpos = [];
	while cmax > 0.1*absMax
		i = maxpos;
		while i<length(chist) && chist(i) > 0.1*absMax
			chist(i) = 0;
			i = i+1;
		end
		
		i = maxpos-1;
		while i> 0 && chist(i) > 0.1*absMax
			chist(i) = 0;
			i = i-1;
		end
		
		centerpos = [centerpos, maxpos];
		[cmax, maxpos] = max(chist);
	end
	centerpos = bins(sort(centerpos));
		
	numClusters = length(centerpos);
	idx = zeros(length(xpos),1);
	for i=1:1:length(xpos)
		[~,idx(i)] = min(abs(cpos(i) - centerpos));
	end
	
	% On each bar-points, do a linear fitting, and report the spacing between (approximately) parallel lines
	m = zeros(numClusters,1);
	b = m;
	scatter(xpos,ypos, [], idx);
	hold on;
	for i=1:1:numClusters
		x = xpos(idx == i);
		y = ypos(idx == i);
		
		if rotflag == false
			p = polyfit(x,y,1);
			m(i) = p(1);
			b(i) = p(2);
			xfit = (min(x)-40):1:(max(x)+40);
			yfit = polyval(p,xfit);
			plot(xfit, yfit, 'k');
		else % If too steep, fitting does not converge. So, a work-around by rotating the coordinate axes.
			p = polyfit(y,x,1);
			m(i) = p(1);
			b(i) = p(2);
			yfit = (min(y)-40):1:(max(y)+40);
			xfit = polyval(p,yfit);
			plot(xfit, yfit, 'k');
		end
		
			
	end
	hold off;
		
	mave = mean(m);
	b1 = b(1:end-1);
	b2 = b(2:end);
	d = abs((b2-b1)*sqrt(1+mave^2));
	set(0,'DefaultAxesFontSize',18);
	title(['distance: ' num2str(mean(d)) '+/-' num2str(std(d,1))]);
	print([cfilename(1:end-4) '_out.png'], '-dpng');
end
