set term png
set output "out.png"
plot "data.dat" using 1:2 title 'map', \
     "data.dat" using 1:3 title 'unordered_map'
