---
name: NEW USERS
about: New users read this to understand how to use
title: NEW USERS RECOMMENDED INTRO
labels: documentation
assignees: ''

---

**Running React.js**
In the terminal in the 'mlb-algo-frontend' folder execute 
- ```NODE_OPTIONS=--openssl-legacy-provider npm start```

**Running the backend API**
In a new terminal in the 'backend' folderexecute 
- ```source venv/bin/activate```
- ```pip install -r requirements.txt```
- ```flask run```

**Using the App**
You can now go to your preferred browser and view the website on 'http://localhost:3000', or whatever port you are running the React.js app on.

**Shutting Down**
Flask
- Ensure you are in the terminal running the Flask app and use <kbd>Ctrl</kbd> + <kbd>C</kbd>
- ```deactivate```
React.js
- Ensure you are in the terminal running the React.js app and use <kbd>Ctrl</kbd> + <kbd>C</kbd>
