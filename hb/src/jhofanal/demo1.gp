set term png
set output "demo1.png"
set xlabel "x axis"
set format x "%03.2f"
set xtics 0.2
set mxtics 0.05
set ylabel "y axis"
set label 100 "simgle plot example" at graph 0.5, graph 0.5
set arrow 100 from graph 0.5, graph 0.5 to graph 1, graph 1
plot \
"data.dat" using 1:2 t 'data' pt 1,\
x
set nolabel 100
set noarrow 100
