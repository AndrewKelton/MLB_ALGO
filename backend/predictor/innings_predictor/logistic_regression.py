import numpy
import pandas as pd
from sklearn import linear_model

if __name__ == "__main__":
    df = pd.read_csv("innings.csv")

    column_szn = df["2024"]
    column_l = df["Last 1"]
    column_l_fixed = []

    X = numpy.array(column_szn).reshape(-1,1)

    for i in range(len(column_l)):
        if column_l[i] >= 1.0:
            column_l_fixed.append(1)
        else:
            column_l_fixed.append(0)
    
    y = numpy.array(column_l_fixed)

    logr = linear_model.LogisticRegression()
    logr.fit(X, y)

    # Need to change this to the game we want to predict NOT 0.45 use the actual team's 2024 val
    predicted = logr.predict(numpy.array([0.45]).reshape(-1, 1))

    print(predicted)


