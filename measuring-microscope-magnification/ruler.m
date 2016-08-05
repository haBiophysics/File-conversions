% Tunc Kayikcioglu, 2016
% Ha Lab, Johns Hopkins University

%% Measures the distance between the bars in the input file

pkg load statistics;
more off;

filename = cell(2,1);
filename{1} = 'x.tif';
filename{2} = 'y.tif';

% Kernel to smooth out the input images
% The larger the kernel is, the more smoothing there will be.
kernelDim = 7;
kernel = 1/(kernelDim^2)*ones(kernelDim, kernelDim);
numTrials = 1e4;
bins = 1:10:1000;
out = zeros(length(filename), length(bins));

for jj=1:1:length(filename)
	cfilename = filename{jj}
	xim = double(imread(cfilename));
	xmin = min(min(xim));
	xmax = max(max(xim));
	xthreshold = (3*xmin+xmax)/4; % 25th percentile
	smooth = conv2(xim,kernel, 'valid');
	xbool = (smooth < xthreshold); % on stick, 1; else 0
	
	[xpos, ypos] = find(xbool);
	chosen = (rand(length(xpos),1) > 0.9); % Randomly select a subset of all pixels to reduce data size.
	xpos = xpos(chosen);
	ypos = ypos(chosen);
	idx = zeros(length(xpos),1);
	
	xhist = histc(xpos, bins);
	yhist = histc(ypos, bins);
	if std(yhist) > std(xhist) % bars along x
		chist = yhist;
		cpos = ypos;
		rotflag = false;
	else
		chist = xhist;
		cpos = xpos;
		rotflag = true;
	end
	

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
	for i=1:1:length(xpos)
		[~,idx(i)] = min(abs(cpos(i) - centerpos));
	end
	
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
	set(0,'DefaultAxesFontSize',16);
	title(['distance: ' num2str(mean(d)) '+/-' num2str(std(d,1))]);
	figure
	% print([cfilename(1:end-4) '_out.png'], '-dpng');
end
