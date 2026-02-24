//Import required modules

//Express framework for web server
var express = require('express'); 
//Node.js module for handling file paths
var path = require('path');
//Middleware to parse cookies
var cookieParser = require('cookie-parser');
//HTTP request logger middleware
var logger = require('morgan');

// Import route modules
var indexRouter = require('./routes/index');
var userRouter = require('./routes/user');
var transactionRouter = require('./routes/transaction');
var cardRouter = require('./routes/card');
var accountAccessRouter = require('./routes/account_access');
var accountRouter = require('./routes/account');
var loginRouter = require('./routes/login');
var currencyRouter = require('./routes/currency');
//JWT Library for authentication
const jwt = require('jsonwebtoken');
//Initialize Express app
var app = express();
//Middleware setup
//Log incoming HTTP requests
app.use(logger('dev'));
//Parse JSON request bodies
app.use(express.json());
//Parse URL-encoded request bodies
app.use(express.urlencoded({ extended: false }));
//Parse cookies
app.use(cookieParser());
//Serve static files
app.use(express.static(path.join(__dirname, 'public')));
//Public routes
//Home page
app.use('/', indexRouter);
//Login
app.use('/login', loginRouter); 

//Protected routes (require JWT authentication)
app.use('/user',authenticateToken, userRouter);
app.use('/transaction',authenticateToken, transactionRouter);
app.use('/cards',authenticateToken, cardRouter);
app.use('/account_access',authenticateToken, accountAccessRouter);
app.use('/accounts',authenticateToken, accountRouter);
app.use('/currency',authenticateToken, currencyRouter);


//JWT authentication middleware
function authenticateToken(req, res, next){
    const authHeader = req.headers['authorization'];
    //Extract token by removing "Bearer" prefix
    const token = authHeader && authHeader.split(' ')[1];
    //No token provided->unauthorized access
    if(token == null) return res.sendStatus(401);
    jwt.verify(token, process.env.MY_TOKEN, function(err,payload){
        if(err){
            //Invalid token
            return res.sendStatus(403);
        }
        req.card_id=payload.card_id;
        next();
    }) 
}

module.exports = app;