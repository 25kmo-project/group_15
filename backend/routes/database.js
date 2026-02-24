const path = require('path');
const mysql = require('mysql2');
const dotenv = require('dotenv');

//load environment variables from .env file
dotenv.config({ path: path.join(__dirname, '..', '.env') });
//create MySQL/MariaDB connection using environment variables
const connection = mysql.createPool({
  host: process.env.DB_HOST,       
  user: process.env.DB_USER,       
  password: process.env.DB_PASSWORD, 
  database: process.env.DB_NAME,   
  port: process.env.DB_PORT || 3306,
  timezone: 'local',
  dateStrings: true
});
module.exports = connection;