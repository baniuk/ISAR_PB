t = [1 2 2 2 2 3 7 10 20 30 30];
median(t)
n=hist(t,1:50);

n_elements = histc(t,1:50);
c_elements = cumsum(n_elements);
bar(1:50,c_elements);
ta = sum(c_elements);

cum = c_elements(1);
for a=2:50
    if cum>ta/2
        zn = a;
        break;
    end
    cum = cum+c_elements(a);
end
c_elements(zn)