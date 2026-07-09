set datafile separator ","

set terminal pdfcairo enhanced font "sans,10" size 5,3.5
set output "draws/time_degree.pdf"

set title "Query Performance: Degree (Log Scale)"
set ylabel "Average Time per Operation ({/Symbol m}s)"
set logscale y
set grid y
set tics back

# Map graph file names to positions on x-axis
graph_idx(name) = (name eq "tiger_map_hawaii.pg" ? 1.0 : \
                   (name eq "planar_embedding1000000.pg" ? 2.0 : \
                    (name eq "worldcitiespop.pg" ? 3.0 : NaN)))

set xtics ("Hawaii" 1, "Planar 1M" 2, "World Cities" 3) nomirror
set ytics nomirror
set key top left

plot \
  "< grep ',adjacency,' ../benchmark_full.csv | grep ',degree,'" \
    using (graph_idx(stringcolumn(1)) - 0.25):8 with boxes ls 1 title "Adjacency List", \
  "< grep ',adjacency,' ../benchmark_full.csv | grep ',degree,'" \
    using (graph_idx(stringcolumn(1)) - 0.25):8:($8-$9 > 0 ? $8-$9 : 1e-6):($8+$9) with yerrorbars lc rgb 'black' pt 0 notitle, \
    \
  "< grep ',k2-tree,' ../benchmark_full.csv | grep ',degree,'" \
    using (graph_idx(stringcolumn(1))):8 with boxes ls 2 title "k2-tree Basic", \
  "< grep ',k2-tree,' ../benchmark_full.csv | grep ',degree,'" \
    using (graph_idx(stringcolumn(1))):8:($8-$9 > 0 ? $8-$9 : 1e-6):($8+$9) with yerrorbars lc rgb 'black' pt 0 notitle, \
    \
  "< grep ',pemb,' ../benchmark_full.csv | grep ',degree,'" \
    using (graph_idx(stringcolumn(1)) + 0.25):8 with boxes ls 3 title "pemb (Turán)", \
  "< grep ',pemb,' ../benchmark_full.csv | grep ',degree,'" \
    using (graph_idx(stringcolumn(1)) + 0.25):8:($8-$9 > 0 ? $8-$9 : 1e-6):($8+$9) with yerrorbars lc rgb 'black' pt 0 notitle
