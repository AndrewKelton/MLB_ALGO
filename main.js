// shell for project

const { exec } = require('child_process')

function runFile(command, name) {
    console.log(`Running: ${name}`);

    exec(command, (error, stdout, stderr) => {
        if (error) {
            console.error(`Error: ${error.message}`)
        }
        if (stderr) {
            console.error(`Stderr: ${stderr}`);
        }
        console.log(`Output: ${stdout}`);
    });
}