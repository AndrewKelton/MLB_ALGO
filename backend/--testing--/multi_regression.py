import pandas
from sklearn import linear_model

def multi():
    df = pandas.read_csv("data.csv")
    X = df[['wins', 'loss']]
    y = df['wins']

    regr = linear_model.LinearRegression()
    regr.fit(X, y)

    print(regr.coef_)