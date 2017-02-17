%% Tunc Kayikcioglu 2017

% Example script modifying a .pdb file.
% The last two columns of a pdb file (occupancy and B-factors) are typically not used by structure viewers such as Pymol or VMD. So it is convenient to embed additional data into that column.
% Note that PDB viewers (VMD) are white-space sensitive. Tabs are not allowed. Number of spaces should be conserved.

structurefile = fopen('1uaa_saa.txt');
for i=1:1:333
	fgetl(structurefile);
end
areadata = zeros(1269,1);
for i=1:1:size(areadata,1)
	line = fgetl(structurefile);
	areadata(i,1) = str2num(sscanf(line, '%*s %*s %*s %*s %*s %*s %*s %*s %*s %s %*s'));
	%ASG  LEU A  312  311    B        Bridge    -78.32    114.33       1.4      1UAA
end
fclose(structurefile);


pdbfile = fopen('1uaa.pdb');
outfile = fopen('tk.pdb', 'w');
%fprintf(outfile, 'REMARK    Altered file.\n');

while feof(pdbfile) == false
	line = fgetl(pdbfile);	
	tag = strtok(line);
	if strcmp(tag,"ATOM") == true	
		fprintf(outfile, '%s', line(1:60));
		fprintf(outfile, '%6.2f           ', 10);
		
		atom = line(end-2);
		for i=1:1:6
			[word, line] = strtok(line);
		end
		index = str2num(word);
		
		fprintf(outfile, '%c\n', atom);
	else
		fprintf(outfile, '%s\n',line);
	end

	% ATOM      1  O5'  DT C   1      65.168 -14.480  73.898  1.00 42.03           O  
end
fclose(outfile);
