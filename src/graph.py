# lineplotFigLegend.py
import numpy as np
import pylab as pl


def main():
    
    # Make x, y arrays for each graph
    x1,y1,x2,y2,x3,y3,x4,y4 = [], [], [], [], [], [], [], []
    x_st,x_end,y_st,y_end,i= 0,1000006,0,5,0
    with open("../output/time.txt", 'r') as data:

        for line in data:
    
            p = line.split()
            
            if i == 0:
           
                x_st,x_end = float(p[0]),float(p[1])
            elif i == 1:
           
                y_st,y_end  = float(p[0]),float(p[1])
            else:
                
                x1.append(float(p[0]))
                x2.append(float(p[0]))
                x3.append(float(p[0]))
                x4.append(float(p[0]))
           
                y1.append(float(p[1]))
                y2.append(float(p[2]))
                y3.append(float(p[3]))
                y4.append(float(p[4]))

            i+=1
                
    
    # use pylab to plot x and y : Give your plots names
    plot1, = pl.plot(x1,y1, linestyle='-', marker='s', color='r')
    plot2, = pl.plot(x2,y2, linestyle='-', marker='o', color='k')
    plot3, = pl.plot(x3,y3, linestyle='-', marker='*', color='b')
    plot4, = pl.plot(x4,y4, linestyle='-', marker='D', color='g')
    
    # give plot a title
    pl.title('Plot of Execution Time vs. Number of Edges')

    # make axis label
    pl.xlabel('Num of Edges --> ')
    pl.ylabel('Execution time --> ')

    print x_st,x_end,y_st,y_end

    #setting up some margin in graph
    x_st -= float(x_st)/100.0
    x_end += float(x_end)/100.0
    y_st -= float(y_st)/100.0
    y_end += float(y_end)/100.0
    
   # print x_st,x_end,y_st,y_end

    # set axis limits
    pl.xlim(x_st, x_end)
    pl.ylim(y_st, y_end)

    # make legend
    pl.legend((plot1, plot2,plot3,plot4), ('prim', 'prim_minPMA' , 'prim_sortPMA','prim_hybridPMA'), 'best', numpoints=1)

    # show the plot on the screen
    pl.show()

main()
