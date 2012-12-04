set term png
set output "out.png"
set xlabel 'Key size'
set ylabel 'Access time in milliseconds'
plot "string-keys.dat" using 1:2 title 'map', \
     "string-keys.dat" using 1:3 title 'unordered_map'
