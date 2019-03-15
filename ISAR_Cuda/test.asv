h = 200;
w = 400;
w_grid = 100;   % szerokoœæ siatki
h_grid = 0;
wieksza = 0;
mniejsza = 0;
ratio = 0;
window = [25 25];
i = ones(h,w);

if h>w
    wieksza = h;
    mniejsza = w;
else
    wieksza = w;
    mniejsza = h;
end

ratio = w/h;
h_grid = floor(w_grid/ratio);

% rozmiar grid w_grid x h_grid

ile_w = floor((w-window(1))/w_grid)-1;    % ile gridów mieœci siê po d³ugoœci ale bez brzegów, -1 bo to s¹ pocz¹tkowe wspó³rzêdne dla grida i rozci¹ga siê on póŸniej dalej
ile_h = floor((h-window(2))/h_grid)-1;

for x=0:w_grid:ile_w*w_grid
    for y=0:h_grid:ile_h*h_grid
        [y+floor(window(2)/2) x+floor(window(1)/2)]
    end
end