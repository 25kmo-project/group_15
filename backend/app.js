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
var app = express();

app.use(logger('dev'));
app.use(express.json());
app.use(express.urlencoded({ extended: false }));
app.use(cookieParser());
app.use(express.static(path.join(__dirname, 'public')));

app.use('/', indexRouter);
app.use('/user', userRouter);
app.use('/transaction', transactionRouter);
app.use('/cards', cardRouter);
app.use('/account_access', accountAccessRouter);
app.use('/account', accountRouter); // määritellään /account-reititin

app.listen(3000); // lisätty serverille portti
console.log('Server is running on port 3000');

//tämä antaa mahdollisuus käyttää tätä tiedostoa muissa tiedostoissa
module.exports = app;
