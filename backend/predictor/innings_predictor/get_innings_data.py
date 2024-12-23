import sys
from bs4 import BeautifulSoup
import requests
import pandas as pd

URL='https://www.teamrankings.com/mlb/stat/1st-inning-runs-per-game' # Website for 1st inning data

def connect_stats(date : str):
    date_url = URL + "?date=" + date

    response = requests.get(date_url)

    if response.status_code == 200:
        soup = BeautifulSoup(response.text,'html.parser')

        table = soup.find('table')
        rows = table.find_all('tr')

        headers = [header.text for header in rows[0].find_all('th')]

        # Extract data
        data = []
        for row in rows[1:]:
            cells = row.find_all('td')
            data.append([cell.text.strip() for cell in cells])

        df = pd.DataFrame(data, columns=headers)

        print(df)
        
        df.to_csv('backend/predictor/innings.csv', index=False)

    else:
        print("Error getting innings data", sys.stderr)

if __name__ == '__main__':
    """
    date=sys.argv[1]
    connect_stats(date) 
    ** Use this instead of below **
    """

    connect_stats("2024-11-01")