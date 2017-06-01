fname = "figure.fig"
 
if ~exist('FLAG_PLOT')
	FLAG_PLOT = 0;
end
 
aa = load(fname);							%% load file into memory, and start climbing down the struct
aName = fieldnames(aa);
bb = getfield(aa,aName{1});
 
cc = bb.children;
dd = cc.children;
 
nItems = length(dd);
kk = 1;								%% counter for plots
jj = 1;								%% counter for text
textItems = {};
 
if FLAG_PLOT == 1
	figure(1);
	hold on;
	col = repmat('rgmcym',1,round(nItems/6)+1);
end


file = fopen("out.txt", "wt");

for ii = 1:nItems
	ee = dd(ii);
 
	if strcmp(ee.type,'graph2d.lineseries')				%% check if 2D data
		ff = ee.properties;
		
		fprintf(file, "%f\t %f\n", [ff.XData; ff.YData]);
		fprintf(file, "\n\n");
	end
end

fclose(file);
