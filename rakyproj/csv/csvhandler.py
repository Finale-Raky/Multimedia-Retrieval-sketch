import numpy as np
#import pandas as pd
import csv 
with open('nor_feature_data.csv', 'w') as csvfilew:
    with open('no_outlier115.csv','r') as csvfile:
        csvwriter = csv.writer(csvfilew)
        test = np.zeros((390, 7), dtype=np.double)
        # df = pd.read_csv('no_outlier115.csv',usecols=[1,2,3,4,5,6])
        # print(df.to_string())
    # Return a reader object which will
        readCSV = csv.reader(csvfile, delimiter=',') 
        for row in readCSV:
            print(row)
            print(row[0], row[1], row[2],row[3],row[4],row[5],row[6],)
            print("\n")
            i = 0
            for i in range(6):
                test[row][i] = row[i]
                
        csvwriter.writerow(row)
 
    