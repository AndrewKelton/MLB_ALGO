export const fetchGames = async () => {
    const response = await fetch('http://localhost:5000/api/games');
    if (!response.ok) {
      throw new Error('Network response was not ok');
    }
    const data = await response.json();
    return data;
  };