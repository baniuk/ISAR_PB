function savebinarymatrix(data,filename)
% saves matrix i binary format accetable by readbinarymatrix command

fid = fopen(filename, 'w');
fwrite(fid,size(data,1),'uint32');
fwrite(fid,size(data,2),'uint32');
data1 = reshape(data',numel(data),[]);
fwrite(fid,data1,'double');
fclose(fid);