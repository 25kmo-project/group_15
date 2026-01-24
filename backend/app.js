var express = require('express');
var path = require('path');
var cookieParser = require('cookie-parser');
var logger = require('morgan');

var indexRouter = require('./routes/index');
var userRouter = require('./routes/user');
var transactionRouter = require('./routes/transaction');
var cardRouter = require('./routes/card');
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


//tämä antaa mahdollisuus käyttää tätä tiedostoa muissa tiedostoissa
module.exports = app;
