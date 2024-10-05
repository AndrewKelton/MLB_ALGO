import logo from './logo.svg';
import React, { useState } from "react"
import './App.css';

const PredictForm = () => {
  const [formData, setFormData] = useState({
    homeTeam: "",
    awayTeam: ""
  });

  const [predict, setPrediction] = useState(null);

  const handleChange = (e) => {
    setFormData({ ...formData, [e.target.name]: e.target.value});
  };

  const handleSubmit = async (e) => {
    e.preventDefault();

    try {
      const response = await fetch("http://localhost:3000/predict", {
        method: "POST",
        headers: {
          "Content-Type": "application/json",
        },
        body: JSON.stringify({
          home_team: formData.homeTeam,
          away_team: formData.awayTeam,
        }),
      });
    
      const data = await Response.json();
      setPrediction(data.prediction === 1 ? "Home team wins!" : "Away team wins!");
    } catch (error) {
      console.error("Error making prediction: ", error)
    }
  };

  return (
    <div>
      <div className='container'>
        <img src="/mlb_algo_logo.jpeg" alt="Logo" className="App-logo"/>
      </div>

      <h2>MLB Game Outcome Predictor</h2>
      <form onSubmit={handleSubmit}>
        <input
          type="text"
          name="homeTeam"
          placeholder="Home Team Name"
          value={formData.homeTeam}
          onChange={handleChange}
        />
        <input
          type="text"
          name="awayTeam"
          placeholder="Away Team Name"
          value={formData.awayTeam}
          onChange={handleChange}
        />
        <button type="submit">Predict Outcome</button>
      </form>

      {predict && <p>{predict}</p>}
    
    </div>
  );
};

export default PredictForm;