// Main frontend file
import React, { useState, useEffect } from "react"
// import { BrowserRouter as Router, Route, Routes } from 'react-router-dom';
import './App.css';


 /* 
  * QUICK FIX 
  * NODE_OPTIONS=--openssl-legacy-provider npm start
  */


// Collect and return today's date (unused currently)
function getDate() {
  const today = new Date();
  const year = today.getFullYear();
  const month = today.getMonth() + 1; // Months are 0-indexed
  const day = today.getDate();

  return {
    year,
    month: month < 10 ? `0${month}` : month,  // Zero-pad single digits
    day: day < 10 ? `0${day}` : day           // Zero-pad single digits
  };
}

// Format date for server-side queries
function formatDate(date) {
  const year = date.getFullYear();
  const month = (date.getMonth() + 1).toString().padStart(2, '0'); // Zero-pad single digits
  const day = date.getDate().toString().padStart(2, '0');          // Zero-pad single digits
  return `${year}-${month}-${day}`;
}

// Get games for date
function GamesList() {
  const [data, setData] = useState(null);
  const [selectedDate, setSelectedDate] = useState(formatDate(new Date())); // Set the default to today

  const fetchGames = (date) => {
    console.log(`Fetching games for date: ${date}`);
    fetch(`http://127.0.0.1:5000/api/games/${date}`)  // Use the selected date in the fetch URL
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
  };

  useEffect(() => {
    fetchGames(selectedDate);  // Fetch games for the initial selected date (today)
  }, [selectedDate]);  // Re-run the effect when the selected date changes

  const handleDateChange = (e) => {
    setSelectedDate(e.target.value);  // Update the selected date
  };

  const renderGame = (game) => {
    return (
      <li key={game.game_id}>
        {game.away_team} @ {game.home_team} | Pick: {game.pick_name}
      </li>
    );
  };

  return (
    <div style={{alignContent:"center"}}>
      <h2>MLB Games for {selectedDate}</h2>

      <div>
      {/* Date input to allow the user to pick a date */}
        <input style={{textAlign: "center"}}
          type="date" 
          value={selectedDate} 
          onChange={handleDateChange} 
        />

        {data ? (
          <ul style={{marginLeft: 10, listStyle: "none"}}>
            {data.map(renderGame)}
          </ul>
        ) : (
          <p>Loading games...</p>
        )}
      </div>
    </div>
  );
}

function GamesNoPick() {
  const [data]
}
// function GamesList() {
//   const [data, setData] = useState(null);
//   const todayDate = getDate();
// 
//   useEffect(() => {
//     console.log("Fetching games...");
//     const formattedDate = `${todayDate.year}-${todayDate.month}-${todayDate.day}`;  // Format the date
// 
//     fetch('http://127.0.0.1:5000/api/games')
//       .then(response => {
//         console.log("Response received:", response);
//         if (!response.ok) {
//           throw new Error('Network response was not ok ' + response.statusText);
//         }
//         return response.json();
//       })
//       .then(data => {
//         console.log("Data received:", data);
//         setData(data);
//       })
//       .catch(error => console.error("Error fetching games: ", error));
//   }, []);

  // const handleClick = (game) => {
  //   console.log(`You clicked on game ${game.game_id}: ${game.away_team} @ ${game.home_team}`);
  //   GameDetails(game);
  //   // window.location.href = `/game/${game.game_id}`;
  //   // setSelectedGame(game);
  // };

//   const renderGame = (game) => {
//     return (
//       <li key={game.game_id}>
//         {/* <button onClick={() => handleClick(game)}> */}
//           {game.away_team} @ {game.home_team} | Pick: {game.pick_name}
//         {/* </button> */}
//       </li>
//     );
//   };
// 
//   return (
//     <div>
//       <h2>MLB Games for {`${todayDate.month}-${todayDate.day}-${todayDate.year}`}</h2>
//       {data ? (
//         <ul>
//           {data.map(renderGame)}
//         </ul>
//       ) : (
//         <p>Loading games...</p>
//       )}
//     </div>
//   );
// }

// Returns some details for a specific game
function GameDetails({ game }) {
  return (
    <div>
      <h1>Game Details</h1>
      <p>Game ID: {game.game_id}</p>
      <p>{game.away_team} @ {game.home_team}</p>
      {/* Add more details as needed */}
    </div>
  );
}

function PredictionsList() {
  const [data, setData] = useState(null);
  // return predicitons for games
}

// Main function
const PredictForm = () => {

  const [predict, setPrediction] = useState(null);

  return (
    <div>
      <div className='container'>
        <img src="/mlb_algo_logo.jpeg" alt="Logo" className="App-logo"/>
      </div>

      <h1 style={{textAlign:"center"}}>MLB Game Outcome Predictor</h1>
      
      <div style={{marginLeft: 30}}>
        <GamesList />
      </div>

    </div>
  );
};

export default PredictForm;