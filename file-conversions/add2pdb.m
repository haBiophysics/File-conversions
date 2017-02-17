%% Tunc Kayikcioglu 2017

% Example script modifying a .pdb file.
% The last two columns of a pdb file (occupancy and B-factors) are typically not used by structure viewers such as Pymol or VMD. So it is convenient to embed additional data into that column.
% Note that PDB viewers (VMD) are white-space sensitive. Tabs are not allowed. Number of spaces should be conserved.

areadata = zeros(1300,2);
structurefile = fopen('1uaa_saa.txt');
while feof(structurefile) == false
	line = fgetl(structurefile);
	tag = strtok(line);
	if strcmp(tag,"ASG") == true
		%[chainid, resid, surfarea] = 
		chainid = sscanf(line, '%*s %*s %s %*s %*s %*s %*s %*s %*s %*s %*s %*s');
		resid = sscanf(line, '%*s %*s %*s %d %*s %*s %*s %*s %*s %*s %*s');
		surfarea = sscanf(line, '%*s %*s %*s %*s %*s %*s %*s %*s %*s %f %*s');
		
		if chainid == 'A'
			areadata(resid,1) = surfarea;
		else
			areadata(resid,2) = surfarea;
		end		
		
		%ASG  ARG A    2    1    C          Coil    360.00   -146.88     138.6      1UAA
	end
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
		
		atom = line(end-2);
		for i=1:1:5
			[word, line] = strtok(line);
		end
		chain = word;
		word = strtok(line);
		resid = str2num(word);
		
		if chain == "A"
			area = areadata(resid,1);
		else
			area = areadata(resid,2);
		end
		
		fprintf(outfile, '%6.2f           %c\n', area, atom);
	else
		fprintf(outfile, '%s\n', line);
	end

	% ATOM      1  O5'  DT C   1      65.168 -14.480  73.898  1.00 42.03           O  
end
fclose(outfile);
