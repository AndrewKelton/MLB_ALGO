import pandas as pd
import matplotlib.pyplot as plt
from sklearn.model_selection import train_test_split
from sklearn.linear_model import LogisticRegression
from sklearn.metrics import classification_report, confusion_matrix

# Load the data
data = {
    "Team": ["Arizona", "LA Dodgers", "NY Yankees", "Philadelphia", "Miami", "San Diego", "Seattle", 
             "Toronto", "Boston", "Baltimore", "Atlanta", "Texas", "Chi Cubs", "Cleveland", "Milwaukee", 
             "Colorado", "St. Louis", "Minnesota", "Oakland", "Houston", "Cincinnati", "Kansas City", 
             "Detroit", "NY Mets", "Tampa Bay", "SF Giants", "Washington", "LA Angels", "Pittsburgh", 
             "Chi Sox"],
    "Home": [0.00, 0.73, 0.61, 0.67, 0.69, 0.48, 0.37, 0.68, 0.67, 0.61, 0.63, 0.48, 0.56, 0.59, 0.61, 
             0.57, 0.46, 0.54, 0.42, 0.53, 0.48, 0.53, 0.52, 0.44, 0.36, 0.46, 0.52, 0.46, 0.47, 0.33],
    "Away": [0.78, 0.57, 0.67, 0.60, 0.53, 0.72, 0.79, 0.43, 0.41, 0.43, 0.41, 0.54, 0.43, 0.39, 0.36, 
             0.39, 0.45, 0.35, 0.47, 0.35, 0.40, 0.33, 0.34, 0.41, 0.48, 0.37, 0.30, 0.31, 0.28, 0.36]
}

df = pd.DataFrame(data)

# Create the binary target variable
df['HomeWin'] = (df['Home'] > df['Away']).astype(int)

# Features and target
X = df[['Home', 'Away']]
y = df['HomeWin']

# Split the data
X_train, X_test, y_train, y_test = train_test_split(X, y, test_size=0.2, random_state=42)

# Logistic Regression model
model = LogisticRegression()
model.fit(X_train, y_train)

# Predictions
y_pred = model.predict(X_test)

# Model evaluation
print("Confusion Matrix:\n", confusion_matrix(y_test, y_pred))
print("\nClassification Report:\n", classification_report(y_test, y_pred))



# Coefficients
coefficients = pd.DataFrame({"Feature": X.columns, "Coefficient": model.coef_[0]})
print("\nCoefficients:\n", coefficients)


plt.scatter(df['Home'], df['Away'], c=df['HomeWin'], cmap='coolwarm')
plt.xlabel('Home Scores')
plt.ylabel('Away Scores')
plt.title('Home vs. Away Scores')
plt.colorbar(label='HomeWin (0 = Away Win, 1 = Home Win)')
plt.show()

# import numpy
# import pandas as pd
# from sklearn import linear_model
# 
# def get_team(home : str, away : str):
#     df = pd.read_csv("innings.csv")
# 
#     home_val = df["Home"][home]
#     away_val = df["Away"][away]
# 
# # recieve inning data from csv
# def get_data():
#     df = pd.read_csv("innings.csv")
# 
#     column_szn = df["Home"]
#     column_l = df["Last 1"]
#     column_l_fixed = []
# 
#     for i in range(len(column_l)):
#         if column_l[i] >= 1.0:
#             column_l_fixed.append(1)
#         else:
#             column_l_fixed.append(0)
#     
#     x = numpy.array(column_szn).reshape(-1,1)
#     y = numpy.array(column_l_fixed)
# 
#     return x, y
# 
# 
# if __name__ == "__main__":
#    
#     X, y = get_data()
# 
#     logr = linear_model.LogisticRegression()
#     logr.fit(X, y)
# 
#     # Need to change this to the game we want to predict NOT 0.45 use the actual team's 2024 val
#     predicted = logr.predict(numpy.array([.82]).reshape(-1, 1))
# 
#     print(predicted)
# 
#     if predicted[0] == 0:
#         print("No run first inning")
#     else:
#         print("Run first inning")
