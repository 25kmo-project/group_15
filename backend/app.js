var express = require('express');
var path = require('path');
var cookieParser = require('cookie-parser');
var logger = require('morgan');

var indexRouter = require('./routes/index');
//luodaan uusi muuttuja ja tallennetaan sinne user.js
var userRouter = require('./routes/user');

var app = express();

app.use(logger('dev'));
app.use(express.json());
app.use(express.urlencoded({ extended: false }));
app.use(cookieParser());
app.use(express.static(path.join(__dirname, 'public')));

app.use('/', indexRouter);
//kaikki pyynnöt jotka alkaavat /user käsittelee userRouter
app.use('/user', userRouter);

//tämä antaa mahdollisuus käyttää tätä tiedostoa muissa tiedostoissa
module.exports = app;
