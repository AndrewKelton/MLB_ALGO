// Main frontend file
import React, { useState, useEffect } from "react"
import './App.css';

 /* 
  * QUICK FIX 
  * NODE_OPTIONS=--openssl-legacy-provider npm start
  */


function getDate() {
  const today = new Date();
  const year = today.getFullYear();
  const month = today.getMonth() + 1; // Months are 0-indexed
  const day = today.getDate();

  return (
    <h>
      {month}-{day}-{year}
    </h>
  );
}

function GamesList() {
  const [data, setData] = useState(null);

  useEffect(() => {
    console.log("Fetching games...");
    fetch('http://localhost:5000/api/games')
      .then(response => {
        console.log("Response received:", response);
        if (!response.ok) {
          throw new Error('Network response was not ok ' + response.statusText);
        }
        return response.json();
      })
      .then(data => {
        console.log("Data received:", data);
        setData(data);
      })
      .catch(error => console.error("Error fetching games: ", error));
  }, []);

  const handleClick = (game) => {
    console.log(`You clicked on game ${game.game_id}: ${game.away_team} @ ${game.home_team}`);
    window.location.href = `/game/${game.game_id}`;
  };

  const renderGame = (game) => {
    return (
      <li key={game.game_id}>
        <button onClick={() => handleClick(game)}>
          {game.away_team} @ {game.home_team}
        </button>
      </li>
    );
  };

  return (
    <div>
      <h2>MLB Games for {getDate()}</h2>
      {data ? (
        <ul>
          {data.map(renderGame)}
        </ul>
      ) : (
        <p>Loading games...</p>
      )}
    </div>
  );
}

const PredictForm = () => {
  const [formData, setFormData] = useState({
    homeTeam: "",
    awayTeam: ""
  });

  const [predict, setPrediction] = useState(null);

  return (
    <div>
      <div className='container'>
        <img src="/mlb_algo_logo.jpeg" alt="Logo" className="App-logo"/>
      </div>

      <h1>MLB Game Outcome Predictor</h1>

      <GamesList />

    </div>
  );
};

export default PredictForm;