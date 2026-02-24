const express = require('express');
const router = express.Router();
const user = require('../models/user_model');

//Get all users
router.get('/', function (request, response){
    user.getAll(function(err, result){
        if(err){
            response.send (err);
        }
        else{
            response.json (result);
        }
    })
});

//Get a single user by user id
router.get('/:user_id', function(request, response){
    user.getOne(request.params.user_id, function(err,result){
        if(err){
            response.send(err);
        }
        else{
            response.json(result[0]);
        }
    })
});

//Add a new user
router.post('/:user_id',function(request,response){
    user.add(request.body,request.params.user_id,function(err,result){
        if(err){
            response.send(err);
        }
        else{
            response.json(result);
        }
    })
});

//Updates user's information
router.put('/:user_id', function(request, response){
    user.update(request.body, request.params.user_id, function(err, result){
        if(err){
            response.send(err);
        }
        else{
            console.log(result.affectedRows);
            response.json(result);
        }
    });
});

//Delet a user
router.delete('/:user_id', function(request, response){
    user.delete(request.params.user_id, function(err, result){
        if(err){
            response.send(err);
        }
        else{
            response.json(result);
        }
    });
});

module.exports=router;