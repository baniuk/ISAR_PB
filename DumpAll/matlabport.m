fid = fopen('test1.out');
ile = fread(fid,1,'uint32');    % ile wpisów
offset = fread(fid,256,'uint32');    % ofsety

for a=1:ile
    typ = fread(fid,1,'uint32');    % typ
    switch(typ)
        case 1        
            size = fread(fid,1,'uint32');    % rozmiar
            data = fread(fid,size,'double',0,'l')';    % dane
            rozmiar_nazwy = fread(fid,1,'uint32');  % rozmiar nazwy
            nazwa_tmp = fread(fid,rozmiar_nazwy,'char')'; % nazwa
            nazwa = char(nazwa_tmp);
            assignin('base',nazwa,data);
    end
end
fclose(fid);