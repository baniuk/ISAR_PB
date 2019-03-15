function ImportDumpFile(nazwa)
% importuje dane z klasy C_DumpAll. Dok³adniejsze inforamce w katalogu projektu
fid = fopen(nazwa);
ile = fread(fid,1,'uint32');    % ile wpisów
offset = fread(fid,256,'uint32');    % ilosci bajtów na ka¿dy wpis

for a=1:ile
    typ = fread(fid,1,'uint32');    % typ
    switch(typ)
        case 1        % double
            size = fread(fid,1,'uint32');    % rozmiar
            data = fread(fid,size,'double',0,'l')';    % dane
            rozmiar_nazwy = fread(fid,1,'uint32');  % rozmiar nazwy
            nazwa_tmp = fread(fid,rozmiar_nazwy,'char')'; % nazwa
            nazwa = char(nazwa_tmp);
            assignin('base',nazwa,data);
        case 2
            row = fread(fid,1,'uint32');    % rozmiar r
            col = fread(fid,1,'uint32');    % rozmiar c
            data = fread(fid,[col,row],'double',0,'l')';    % dane
            rozmiar_nazwy = fread(fid,1,'uint32');  % rozmiar nazwy
            nazwa_tmp = fread(fid,rozmiar_nazwy,'char')'; % nazwa
            nazwa = char(nazwa_tmp);
            assignin('base',nazwa,data);
        case 3
            row = fread(fid,1,'uint32');    % rozmiar r
            col = fread(fid,1,'uint32');    % rozmiar c
            data = fread(fid,[col,row],'double',0,'l')';    % dane
            rozmiar_nazwy = fread(fid,1,'uint32');  % rozmiar nazwy
            nazwa_tmp = fread(fid,rozmiar_nazwy,'char')'; % nazwa
            nazwa = char(nazwa_tmp);
            assignin('base',nazwa,data);
		case 4        % float
            size = fread(fid,1,'uint32');    % rozmiar
            data = fread(fid,size,'float',0,'l')';    % dane
            rozmiar_nazwy = fread(fid,1,'uint32');  % rozmiar nazwy
            nazwa_tmp = fread(fid,rozmiar_nazwy,'char')'; % nazwa
            nazwa = char(nazwa_tmp);
            assignin('base',nazwa,data);
        otherwise
            disp('Type not supported');
    end
end
fclose(fid);