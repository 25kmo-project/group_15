var express = require('express');
var path = require('path');
var cookieParser = require('cookie-parser');
var logger = require('morgan');

var indexRouter = require('./routes/index');
var userRouter = require('./routes/user');
var transactionRouter = require('./routes/transaction');
var cardRouter = require('./routes/card');
var accountAccessRouter = require('./routes/account_access');
var accountRouter = require('./routes/account'); // lisätty account-reititin
//login
var loginRouter = require('./routes/login');
const jwt = require('jsonwebtoken');

var app = express();

app.use(logger('dev'));
app.use(express.json());
app.use(express.urlencoded({ extended: false }));
app.use(cookieParser());
app.use(express.static(path.join(__dirname, 'public')));

app.use('/', indexRouter);
app.use('/login', loginRouter); 

//suojatut reitit
//esimerkki
// app.use('/user',authenticateToken, userRouter);
app.use('/user', userRouter);
app.use('/transaction', transactionRouter);
app.use('/cards', cardRouter);
app.use('/account_access', accountAccessRouter);
app.use('/account', accountRouter); // määritellään /account-reititin


//tarkisus jwt(jason web token)
function authenticateToken(req, res, next){
    const authHeader = req.headers['authorization'];
    //Extract token by removing "Bearer" prefix
    const token = authHeader && authHeader.split(' ')[1];

    if(token == null) return res.sendStatus(401);
    jwt.verify(token, process.env.MY_TOKEN, function(err,payload){
        if(err){
            return res.sendStatus(403);
        }
        req.card_id=payload.card_id;
        next();
    }) 
}

//tämä antaa mahdollisuus käyttää tätä tiedostoa muissa tiedostoissa
module.exports = app;
