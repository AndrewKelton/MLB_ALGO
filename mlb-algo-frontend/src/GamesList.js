import React, { useState, useEffect } from 'react';

function GamesList() {
  const [data, setData] = useState(null);

  useEffect(() => {
    fetch('http://localhost:5000/api/games')
        .then(response => response.json())
        .then(data => setData(data))
        .catch(error => console.error("Error fetching games: ", error));
    }, []); // Empty dependency array means it runs once when component mounts


  return (
    <div>
      <h2>MLB Games</h2>
      <ul>
        {data.games.map(game => (
          <li key={game.id}>
            {game.home_team} vs {game.away_team}
          </li>
        ))}
      </ul>
    </div>
  );
};

export default GamesList;
